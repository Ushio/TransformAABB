#include "pr.hpp"
#include <iostream>
#include <memory>

// Graphics Gems X.8 TRANSFORMING AXIS-ALIGNED BOUNDING BOXES
void AABB_ApplyTransform( glm::vec3 lower, glm::vec3 upper, glm::mat4 m, glm::vec3* newLower, glm::vec3* newUpper )
{
	for( int i = 0; i < 3; ++i )
	{
		( *newLower )[i] = m[3][i];
		( *newUpper )[i] = m[3][i];

		for( int j = 0; j < 3; ++j )
		{
			float a = m[j][i] * lower[j];
			float b = m[j][i] * upper[j];

			( *newLower )[i] += std::min( a, b );
			( *newUpper )[i] += std::max( a, b );
		}
	}
}

int main()
{
	using namespace pr;

	Config config;
	config.ScreenWidth = 1920;
	config.ScreenHeight = 1080;
	config.SwapInterval = 1;
	Initialize( config );

	Camera3D camera;
	camera.origin = { 4, 4, 4 };
	camera.lookat = { 0, 0, 0 };
	camera.zUp = false;

	glm::vec3 AABB_min = { -1, -1, -1 };
	glm::vec3 AABB_max = { +1, +1, +1 };

	glm::vec3 scaling = { 1, 1, 1 };
	glm::vec3 rotation = { 0, 0, 0 };
	glm::vec3 translation = { 0, 0, 0 };

	double e = GetElapsedTime();

	while( pr::NextFrame() == false )
	{
		if( IsImGuiUsingMouse() == false )
		{
			UpdateCameraBlenderLike( &camera );
		}

		ClearBackground( 0.1f, 0.1f, 0.1f, 1 );

		BeginCamera( camera );

		PushGraphicState();

		DrawGrid( GridAxis::XZ, 1.0f, 10, { 128, 128, 128 } );
		DrawXYZAxis( 1.0f );

		glm::mat4 m;
		m = glm::translate( m, translation );
		m = glm::rotate( m, rotation.x, { 1, 0, 0 } );
		m = glm::rotate( m, rotation.z, { 0, 0, 1 } );
		m = glm::rotate( m, rotation.y, { 0, 1, 0 } );
		m = glm::scale( m, scaling );

		SetObjectTransform( m );
		DrawAABB( AABB_min, AABB_max, { 255, 0, 0 } );
		SetObjectIdentify();

		glm::vec3 newLower, newUpper;
		AABB_ApplyTransform( AABB_min, AABB_max, m, &newLower, &newUpper );
		DrawAABB( newLower, newUpper, { 255, 255, 255 } );

		PopGraphicState();
		EndCamera();

		BeginImGui();

		ImGui::SetNextWindowSize( { 500, 800 }, ImGuiCond_Once );
		ImGui::Begin( "Panel" );
		ImGui::Text( "fps = %f", GetFrameRate() );

		ImGui::SliderFloat3( "AABB min", glm::value_ptr( AABB_min ), -5, 5 );
		ImGui::SliderFloat3( "AABB max", glm::value_ptr( AABB_max ), -5, 5 );
		ImGui::SliderFloat3( "Scaling", glm::value_ptr( scaling ), 0, 3 );
		ImGui::SliderFloat3( "Rotation", glm::value_ptr( rotation ), 0, glm::two_pi<float>() );
		ImGui::SliderFloat3( "Translation", glm::value_ptr( translation ), -5, 5 );

		static Stopwatch sw;
		static bool animR = false;
		static bool animT = false;
		static bool animS = false;
		float speed = 0.1f;
		if( animT )
		{
			translation.x = glm::simplex( glm::vec4{ sw.elapsed() * speed, 0, 0, 0 } ) * glm::two_pi<float>();
			translation.y = glm::simplex( glm::vec4{ 0, sw.elapsed() * speed, 0, 0 } ) * glm::two_pi<float>();
			translation.z = glm::simplex( glm::vec4{ 0, 0, sw.elapsed() * speed, 0 } ) * glm::two_pi<float>();
		}
		if( animR )
		{
			rotation.x = glm::simplex( glm::vec4{ sw.elapsed() * speed, 0, 0, 9.0f } ) * glm::two_pi<float>();
			rotation.y = glm::simplex( glm::vec4{ 0, sw.elapsed() * speed, 0, 9.0f } ) * glm::two_pi<float>();
			rotation.z = glm::simplex( glm::vec4{ 0, 0, sw.elapsed() * speed, 9.0f } ) * glm::two_pi<float>();
		}
		if( animS )
		{
			scaling.x = 0.1f + glm::simplex( glm::vec4{ sw.elapsed() * speed, 0, 0, 0.3f } ) * 2.0f;
			scaling.y = 0.1f + glm::simplex( glm::vec4{ 0, sw.elapsed() * speed, 0, 0.3f } ) * 2.0f;
			scaling.z = 0.1f + glm::simplex( glm::vec4{ 0, 0, sw.elapsed() * speed, 0.3f } ) * 2.0f;
		}
		ImGui::Checkbox( "Animation T", &animT );
		ImGui::Checkbox( "Animation R", &animR );
		ImGui::Checkbox( "Animation S", &animS );

		ImGui::End();

		EndImGui();
	}

	pr::CleanUp();
}
