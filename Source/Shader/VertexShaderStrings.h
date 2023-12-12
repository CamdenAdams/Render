


struct ShaderStruct {

	ShaderStruct() {}

	const char* sceneVertexSource = R"glsl(
			#version 150 core
			in vec3 position;
			in vec3 color;
			in vec2 texcoord;

			out vec2 Texcoord;
			out vec3 Color;

			uniform mat4 model;
			uniform mat4 view;
			uniform mat4 proj;
			uniform vec3 overrideColor;

			void main()
			{
				Color = overrideColor * color;
				Texcoord = texcoord;
				gl_Position = proj * view * model * vec4(position, 1.0);
			}
		)glsl";

	const char* sceneFragmentSource = R"glsl(
			#version 150 core
			in vec2 Texcoord;
			in vec3 Color;
			out vec4 outColor;
			uniform sampler2D texKitten;
			uniform sampler2D texPuppy;
			void main()
			{
				outColor = vec4(Color, 1.0) * mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), 0.5);
			}
		)glsl";

	const char* screenVertexSource = R"glsl(
			#version 150 core
			in vec2 position;
			in vec2 texcoord;
			out vec2 Texcoord;
			void main()
			{
				Texcoord = texcoord;
				gl_Position = vec4(position, 0.0, 1.0);
			}
		)glsl";

	const char* screenFragmentSource = R"glsl(
			#version 150 core
			in vec2 Texcoord;
			out vec4 outColor;
			uniform sampler2D texFramebuffer;
			void main()
			{
				outColor = texture(texFramebuffer, Texcoord);
			}
		)glsl";
};