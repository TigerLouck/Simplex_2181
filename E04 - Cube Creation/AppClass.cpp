#include "AppClass.h"
void Application::InitVariables(void)
{
	
	for (size_t i = 0; i < 11; i++)
	{
		for (size_t j = 8; j > 0; j--)
		{
			if (spaceInvader[j][i] == 1)
			{
				//Make MyMesh object
				m_pMesh = new MyMesh();
				//Generate a circle
				m_pMesh->GenerateCube(2.0f, C_BLACK);
				matrix4 t = glm::translate(vector3(2 * i, 2 * j, 0));
				matrix4* translation = new matrix4(t);
				cubes.push_back(m_pMesh);
				cubestransform.push_back(translation);
			}
		}
	}
	
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//Render the meshes
	for (size_t i = 0; i < cubes.size(); i++)
	{
		cubes[i]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), *cubestransform[i]);
	}
	//m_pMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), ToMatrix4(m_qArcBall));
		
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//Safely release the memory
	SafeDelete(m_pMesh);
	//release GUI
	ShutdownGUI();
}