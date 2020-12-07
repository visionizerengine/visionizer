#include "vspch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Visionizer {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case Visionizer::ShaderDataType::Float:    return GL_FLOAT;
			case Visionizer::ShaderDataType::Float2:   return GL_FLOAT;
			case Visionizer::ShaderDataType::Float3:   return GL_FLOAT;
			case Visionizer::ShaderDataType::Float4:   return GL_FLOAT;
			case Visionizer::ShaderDataType::Mat3:     return GL_FLOAT;
			case Visionizer::ShaderDataType::Mat4:     return GL_FLOAT;
			case Visionizer::ShaderDataType::Int:      return GL_INT;
			case Visionizer::ShaderDataType::Int2:     return GL_INT;
			case Visionizer::ShaderDataType::Int3:     return GL_INT;
			case Visionizer::ShaderDataType::Int4:     return GL_INT;
			case Visionizer::ShaderDataType::Bool:     return GL_BOOL;
		}

		VS_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		VS_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

}