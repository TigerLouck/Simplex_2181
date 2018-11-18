#include "MyOctant.h"
using namespace Simplex;
uint MyOctant::m_nCount = 0;
//  MyOctant
void MyOctant::Init(void)
{
	m_nData = 0;
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_ContainedEnts = std::vector<MyEntity*>();
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
}

MyOctant::MyOctant(int a_iLim)
{
	Init();
	m_iLim = a_iLim;
	std::vector<MyEntity*> l_Entity_List = m_pEntityMngr->GetEntityList();
	uint iEntityCount = l_Entity_List.size();
	std::vector<vector3> v3MaxMin_list;
	for (uint i = 0; i < iEntityCount; ++i)
	{
		

		MyRigidBody* pRG = l_Entity_List[i]->GetRigidBody();
		vector3 v3Min = pRG->GetMinGlobal();
		vector3 v3Max = pRG->GetMaxGlobal();
		v3MaxMin_list.push_back(v3Min);
		v3MaxMin_list.push_back(v3Max);

		m_ContainedEnts.push_back(l_Entity_List[i]);//hey. cool. nice.
	}

	m_pRigidBody = new MyRigidBody(v3MaxMin_list);
	m_pRigidBody->MakeCubic();
	m_iID = m_nCount;
	Subdivide();
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();
	std::vector<vector3> v3MaxMin_list;
	v3MaxMin_list.push_back(a_v3Center - vector3(a_fSize));
	v3MaxMin_list.push_back(a_v3Center + vector3(a_fSize));
	m_pRigidBody = new MyRigidBody(v3MaxMin_list);
	m_nCount++;
	m_iID = m_nCount;
}

void MyOctant::Subdivide()
{
	if (m_nLevel > 2) {
		//if we're in too deep, set the contained entities' dimensions and back out
		for (size_t j = 0; j < m_ContainedEnts.size(); j++)
		{
			m_pEntityMngr->AddDimension(m_ContainedEnts[j]->GetUniqueID(), m_iDim);
		}
		return;
	}

	//make new children
	vector3 v3Center = m_pRigidBody->GetCenterLocal();
	vector3 v3HalfWidth = m_pRigidBody->GetHalfWidth();
	float fSize = (v3HalfWidth.x) / 2.0f;
	float fCenters = fSize;

	m_pChild[0] = new MyOctant(v3Center + vector3(fCenters, fCenters, fCenters), fSize);
	m_pChild[1] = new MyOctant(v3Center + vector3(-fCenters, fCenters, fCenters), fSize);
	m_pChild[2] = new MyOctant(v3Center + vector3(-fCenters, -fCenters, fCenters), fSize);
	m_pChild[3] = new MyOctant(v3Center + vector3(fCenters, -fCenters, fCenters), fSize);

	m_pChild[4] = new MyOctant(v3Center + vector3(fCenters, fCenters, -fCenters), fSize);
	m_pChild[5] = new MyOctant(v3Center + vector3(-fCenters, fCenters, -fCenters), fSize);
	m_pChild[6] = new MyOctant(v3Center + vector3(-fCenters, -fCenters, -fCenters), fSize);
	m_pChild[7] = new MyOctant(v3Center + vector3(fCenters, -fCenters, -fCenters), fSize);

	//automation variables i'm allocating here for efficiency
	uint iSubCollisions;
	std::vector<MyEntity*> vSubBodies;

	//main construction loop
	for (uint i = 0; i < 8; i++)
	{
		//certain inits that have to be done here
		m_pChild[i]->m_nLevel = m_nLevel + 1;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_iLim = m_iLim;
		m_pChild[i]->m_iDim = m_iDim + i;

		//reset automation
		iSubCollisions = 0;
		vSubBodies = std::vector<MyEntity*>();
		for (size_t j = 0; j < m_ContainedEnts.size(); j++)
		{
			//m_ContainedEnts[j]->SetColorNotColliding(C_BLUE);
			//if an entity from the parent is colliding with a child, add it to the child's contained entities
			if (m_pChild[i]->m_pRigidBody->IsColliding(m_ContainedEnts[j]->GetRigidBody()))
			{
				//std::cout << "found a collision on the octree.";
				iSubCollisions++;
				vSubBodies.push_back(m_ContainedEnts[j]);
				//m_ContainedEnts[j]->SetColorNotColliding(C_BLUE);//testing to see what the octree sees
			}
		}
		//we now know what's in this child's bounds, so let's set that
		m_pChild[i]->m_ContainedEnts = vSubBodies;

		//recursive "call core." this is where the octree call stack is recoursed
		//if we're over the limit, attempt to subdivide, otherwise...
		if (iSubCollisions > m_iLim) 
		{
			m_pChild[i]->Subdivide();
		}
		else 
		{
			//...not over the limit, so add dimensions and return
			for (size_t j = 0; j < m_pChild[i]->m_ContainedEnts.size(); j++)
			{
				m_pEntityMngr->AddDimension(m_pChild[i]->m_ContainedEnts[j]->GetUniqueID(), m_iDim);
			}
		}
	}
}

void MyOctant::Swap(MyOctant& other)
{
	std::swap(m_nData, other.m_nData);
	std::swap(m_lData, other.m_lData);
}
void MyOctant::Release(void)
{
	m_lData.clear();
}
void Simplex::MyOctant::Display(void)
{
	m_pRigidBody->AddToRenderList();
	for (uint i = 0; i < 8; i++)
	{
		if (m_pChild[i])
			m_pChild[i]->Display();
	}
	//m_pMeshMngr->AddWireCubeToRenderList(glm::scale(vector3(70)), C_BLUE);
}
void Simplex::MyOctant::IsColliding(void)
{
	std::vector<MyEntity*> l_Entity_List = m_pEntityMngr->GetEntityList();
	uint iEntityCount = l_Entity_List.size();
	for (uint i = 0; i < iEntityCount; ++i)
	{
		MyRigidBody* pRB = l_Entity_List[i]->GetRigidBody();
		if (pRB->IsColliding(m_pRigidBody))
		{
			l_Entity_List[i]->AddDimension(m_iID);
		}
	}
}
MyOctant::MyOctant(MyOctant const& other)
{
	m_nData = other.m_nData;
	m_lData = other.m_lData;
}
MyOctant& MyOctant::operator=(MyOctant const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}
MyOctant::~MyOctant(){Release();};
//Accessors
void MyOctant::SetData(int a_nData){ m_nData = a_nData; }
int MyOctant::GetData(void){ return m_nData; }
void MyOctant::SetDataOnVector(int a_nData){ m_lData.push_back(a_nData);}
int& MyOctant::GetDataOnVector(int a_nIndex)
{
	int nIndex = static_cast<int>(m_lData.size());
	assert(a_nIndex >= 0 && a_nIndex < nIndex);
	return m_lData[a_nIndex];
}
//--- Non Standard Singleton Methods

