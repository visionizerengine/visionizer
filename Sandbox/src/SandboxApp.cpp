#include <Visionizer.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class ExampleLayer : public Visionizer::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		m_VertexArray.reset(Visionizer::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		Visionizer::Ref<Visionizer::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Visionizer::VertexBuffer::Create(vertices, sizeof(vertices)));
		Visionizer::BufferLayout layout = {
			{ Visionizer::ShaderDataType::Float3, "a_Position" },
			{ Visionizer::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		Visionizer::Ref<Visionizer::IndexBuffer> indexBuffer;
		indexBuffer.reset(Visionizer::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Visionizer::VertexArray::Create());

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Visionizer::Ref<Visionizer::VertexBuffer> squareVB;
		squareVB.reset(Visionizer::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Visionizer::ShaderDataType::Float3, "a_Position" },
			{ Visionizer::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Visionizer::Ref<Visionizer::IndexBuffer> squareIB;
		squareIB.reset(Visionizer::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);


		// Setting up the shaders
		// [TODO] Make better
		m_Shader = Visionizer::Shader::Create("assets/shaders/TriangleShader.glsl");
		m_FlatColorShader = Visionizer::Shader::Create("assets/shaders/FlatColorShader.glsl");
		auto TextureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Visionizer::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernoLogoTexture = Visionizer::Texture2D::Create("assets/textures/ChernoLogo.png");

		std::dynamic_pointer_cast<Visionizer::OpenGLShader>(TextureShader)->Bind();
		std::dynamic_pointer_cast<Visionizer::OpenGLShader>(TextureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Visionizer::Timestep ts) override
	{
		if (Visionizer::Input::IsKeyPressed(VKEY_A))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		else if (Visionizer::Input::IsKeyPressed(VKEY_D))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;

		if (Visionizer::Input::IsKeyPressed(VKEY_W))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		else if (Visionizer::Input::IsKeyPressed(VKEY_S))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;

		if (Visionizer::Input::IsKeyPressed(VKEY_E))
			m_CameraRotation += m_CameraRotationSpeed * ts;
		if (Visionizer::Input::IsKeyPressed(VKEY_Q))
			m_CameraRotation -= m_CameraRotationSpeed * ts;


		Visionizer::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Visionizer::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Visionizer::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Visionizer::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Visionizer::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Visionizer::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		Visionizer::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_ChernoLogoTexture->Bind();
		Visionizer::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		// Triangle
		// Visionizer::Renderer::Submit(m_Shader, m_VertexArray);

		Visionizer::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Visionizer::Event& event) override
	{
	}
private:
	Visionizer::ShaderLibrary m_ShaderLibrary;
	Visionizer::Ref<Visionizer::Shader> m_Shader;
	Visionizer::Ref<Visionizer::VertexArray> m_VertexArray;

	Visionizer::Ref<Visionizer::Shader> m_FlatColorShader;
	Visionizer::Ref<Visionizer::VertexArray> m_SquareVA;

	Visionizer::Ref<Visionizer::Texture2D> m_Texture, m_ChernoLogoTexture;

	Visionizer::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};

class Sandbox : public Visionizer::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}

};

Visionizer::Application* Visionizer::CreateApplication()
{
	return new Sandbox();
}