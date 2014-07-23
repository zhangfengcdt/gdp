// program_gen_dp.cpp
// Generic Dynamic Programming Framework
// by using C++/STL Hash Map <1.0>
// ver 2.0 should include dynamic memory 
// allocation for state and control 
// variables.
// Feng Zhang <fzhang@gatech.edu>
// June, 2009
//

#include "stdafx.h"
#include "program_gen_dp.h"
#include <hash_map>

using namespace stdext; 
using namespace std;
#include <string> 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object
CWinApp theApp;

#define NUM_STATE_DIM	1
#define NUM_CONTROL_DIM	1
#define NUM_STAGE_DIM   11
#define NUM_MAX_CONTIE	2

//Node for DP Algorithm
class CDPNode { 
public : 
  CDPNode( int stage = 0 ) 
    : mStage( stage ){} 
	//keys
	int   mStage; 
	float mState[NUM_STATE_DIM];
	int   mTieIndex;
	//links
	float mControl[NUM_CONTROL_DIM];
	//value
	float fOptimValue;
}; 

namespace stdext { 
     size_t hash_value( const CDPNode& that ) 
     { 
		 size_t ret_size = hash_value( that.mStage) ^ hash_value( that.mTieIndex);
		 for(int i=0; i<NUM_STATE_DIM; i++)
			 ret_size = ret_size ^ hash_value( that.mState[i]);
		 return ret_size; 
     } 
}; 

bool operator<( const CDPNode& that1, const CDPNode& that2) 
{ 
	if(that1.mStage < that2.mStage)
		return true;
	else if(that1.mStage > that2.mStage)
		return false;
	else ;

	if(that1.mTieIndex < that2.mTieIndex)
		return true;
	else if(that1.mTieIndex > that2.mTieIndex)
		return false;
	else ;

	for(int i=0; i<NUM_STATE_DIM; i++)
	{
		if(that1.mState[i] < that2.mState[i])
			return true;
		else if(that1.mState[i] < that2.mState[i])
			return false;
		else ;
	}

	return false; 
}

/////////////////////
//Auxiliary Functions
//Auxiliary Functions
/////////////////////

///////////////////////////////////////
//Global Hash Map to store DP solutions
hash_map<CDPNode, float> GeneralDPTable;
///////////////////////////////////////

float w[NUM_STAGE_DIM] = {0, 1, 2, 1, 0, 0, 1, 2, 1, 0, 0};

//DP Dynamic Function
bool DPSystemFunction(CDPNode& CurrNode, CDPNode& NextNode, float userFlag)
{
	if(CurrNode.mStage >= NUM_STAGE_DIM-1)
		return false;

	//TODO! Need to add interpolation function to make sure
	//the state variables are within satte-set
	NextNode.mStage    = CurrNode.mStage + 1;
	NextNode.mState[0] = CurrNode.mState[0] - CurrNode.mControl[0] + w[CurrNode.mStage];
	return true;
}

//DP Cost-to-Go Function
bool DPStepCostFunction(CDPNode& Node, float *fResult, float userFlag)
{
	if(!fResult)
		return false;
	
	if(Node.mStage == NUM_STAGE_DIM -1)
	{
		*fResult = 0.5*(Node.mState[0] - 8)*(Node.mState[0] - 8);
		return true;
	}
	else if(Node.mStage < NUM_STAGE_DIM -1)
	{
		float fCurrStepValue = -0.1*Node.mState[0]*Node.mControl[0];
		CDPNode NextNode;
		
		if(!DPSystemFunction(Node, NextNode, 0))
			return false;

		CDPNode *pFindNode = new CDPNode;
		pFindNode->mStage      = Node.mStage + 1;
		pFindNode->mState[0]   = NextNode.mState[0];
		pFindNode->mTieIndex   = 1;

		hash_map<CDPNode, float>::iterator it;
		it = GeneralDPTable.find(*pFindNode);

		if(it != GeneralDPTable.end())
		{
			CDPNode retNode = it->first;
			*fResult = fCurrStepValue + retNode.fOptimValue;
			return true;
		}
		else
			return false;
	}
	else 
		return false;

	return true;
}

//DP Feasible Function - State Variables only
bool DPFeasibleState(CDPNode& Node, float userFlag)
{
	if( Node.mStage == NUM_STAGE_DIM -1 )
	{//final states
		if(Node.mState[0] >= 6 && Node.mState[0] <= 8)
			return true;
		else
			return false;
	}
	else if(Node.mStage < NUM_STAGE_DIM -1)
	{
		if(Node.mState[0] >= 3 && Node.mState[0] <= 8)
			return true;
		else
			return false;
	}
	else
		return false;

	return false;
}

//DP Feasible Function - State and Control Variables
bool DPFeasibleControl(CDPNode& Node, float userFlag)
{
	if( Node.mStage < NUM_STAGE_DIM )
	{
		if(Node.mControl[0] >= 0 && Node.mControl[0] <= 3)
			return true;
		else
			return false;
	}
	else return false;

	return false;
}

//main function for test
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		///////////////////////////////////////////////
		//Define a DP Problem
		///////////////////////////////////////////////
		int nDimState; int nDimStage; int nDimControl;
		int nStateNum; int nControlNum;
		float *fStateList= NULL, *fControlList= NULL;
		
		nDimState   = NUM_STATE_DIM;  
		nDimControl = NUM_CONTROL_DIM; 
		nDimStage   = NUM_STAGE_DIM;	
		nStateNum   = 6;  nControlNum = 3;
		fStateList   = (float*)malloc(sizeof(float)*nStateNum*nDimState);
		fControlList = (float*)malloc(sizeof(float)*nControlNum*nDimControl);

		///////////////////////////////////////////////
		//DP Steps 1-3
		///////////////////////////////////////////////

		///////////////////////////////////////////////
		//Step1: Discritize State and Control Variables
		//+Initialize Storage (Hash-Map in this case)
		///////////////////////////////////////////////
		float fStateMin,   fStateMax;
		float fControlMin, fControlMax;
		fStateMin = 3;   fStateMax   = 8; 
		fControlMin = 0; fControlMax = 2;
		for(int i=0; i<nDimState; i++)
		{
			for(int j=0; j<nStateNum; j++)
			{
				*(fStateList+i*nStateNum+j) = fStateMin + 
					(fStateMax - fStateMin)/float(nStateNum-1)*(float)j;
			}
		}
		for(int i=0; i<nDimControl; i++)
		{
			for(int j=0; j<nControlNum; j++)
			{
				*(fControlList+i*nControlNum+j) = fControlMin + 
					(fControlMax - fControlMin)/float(nControlNum-1)*(float)j;
			}
		}

		//Create Final State's Nodes
		for(int j=0; j<nStateNum; j++)
		{
			CDPNode *pNode		= new CDPNode;
			pNode->mStage		= nDimStage-1;
			pNode->mTieIndex	= 1;

			//retrieve a state-set
			for(int i=0; i<nDimState; i++)
				pNode->mState[i]	= *(fStateList+i*nStateNum+j);
			for(int i=0; i<nDimControl; i++)
				pNode->mControl[i]	= 0.0f;

			//check state-set feasibility
			if(DPFeasibleState(*pNode, 0) != false)
			{
				//calculate cost function
				float fCostValue;
				if(DPStepCostFunction(*pNode, &fCostValue, 0) != false)
				{
					pNode->fOptimValue = fCostValue;
					//add this node to hash map
					GeneralDPTable[ *pNode ] = pNode->fOptimValue;
				}
				else
					free(pNode);
			}
			else
				free(pNode);
		}

		//printf("Step 1 finishes!\n");

		///////////////////////////////////////////////
		//Step2: Doing backward searching
		///////////////////////////////////////////////
		for(int iStage=nDimStage-2; iStage>=0; iStage--)
		{
			for(int j=0; j<nStateNum; j++)
			{
				int     iTieIndex   = -1;
				CDPNode *pNode		= new CDPNode;
				pNode->mStage		= iStage;
				pNode->mTieIndex	= iTieIndex;

				for(int i=0; i<nDimState; i++)
					pNode->mState[i]	= *(fStateList+i*nStateNum+j);
					
				//check state feasibility
				if(DPFeasibleState(*pNode, 0) != false)
				{
					//local varible recording optimized comb.
					float fControlOptTie[NUM_MAX_CONTIE][NUM_CONTROL_DIM];
					float fStateOptTie[NUM_MAX_CONTIE][NUM_STATE_DIM];
					float fMinCostFunction          = 0.0f;
					bool  iFlagFirstFeasibleControl = true;

					iTieIndex   = -1;
					//look into each control variable
					for(int l=0; l<nControlNum; l++)
					{
						for(int k=0; k<nDimControl; k++)
							pNode->mControl[k] = *(fControlList+k*nControlNum+l);

					if(iStage == 9 && pNode->mState[0] == 8.0)
						printf("");

						//check state-control feasibility
						if(DPFeasibleControl(*pNode, 0) != false)
						{
							//calculate cost function
							float fCostValue;
							if(DPStepCostFunction(*pNode, &fCostValue, 0) != false)
							{
								if(iFlagFirstFeasibleControl)
								{
									iTieIndex = 1;
									for(int k=0; k<nDimControl; k++)
										fControlOptTie[iTieIndex][k] = pNode->mControl[k];

									iFlagFirstFeasibleControl = false;
									
									for(int k=0; k<nDimState; k++)
										fStateOptTie[iTieIndex][k] = pNode->mState[k];
									for(int k=0; k<nDimControl; k++)
										fControlOptTie[iTieIndex][k] = pNode->mControl[k];
									
									fMinCostFunction          = fCostValue;
								}
								else
								{
									if(fCostValue < fMinCostFunction)
									{//found new lowest
										iTieIndex = 1;

										for(int k=0; k<nDimState; k++)
											fStateOptTie[iTieIndex][k] = pNode->mState[k];
										for(int k=0; k<nDimControl; k++)
											fControlOptTie[iTieIndex][k] = pNode->mControl[k];

										fMinCostFunction = fCostValue;
									}
									else if(fCostValue == fMinCostFunction)
									{//found a tie
										iTieIndex ++;
										if(iTieIndex < NUM_MAX_CONTIE)
										{
											for(int k=0; k<nDimState; k++)
												fStateOptTie[iTieIndex][k] = pNode->mState[k];
											for(int k=0; k<nDimControl; k++)
													fControlOptTie[iTieIndex][k] = pNode->mControl[k];
										}
										else
											iTieIndex --;
									}
								}
							}
						}//if(DPFeasibleControl)
					}//for(l)
					//add all tie nodes into hash map
					if(iTieIndex>=0)
					{
						for(int iTie=1; iTie<=iTieIndex; iTie++)
						{
							CDPNode *pTieNode		   = new CDPNode;
							pTieNode->mStage		   = pNode->mStage;
							pTieNode->mTieIndex        = iTie;
							
							for(int i=0; i<nDimState; i++)
								pTieNode->mState[i]	   = fStateOptTie[iTie][i];
							for(int i=0; i<nDimControl; i++)
								pTieNode->mControl[i]  = fControlOptTie[iTie][i];
							
							pTieNode->fOptimValue      = fMinCostFunction;
							GeneralDPTable[ *pTieNode ]= pTieNode->fOptimValue;
						}
					}
				}//if(DPFeasibleState)

				free(pNode);

			}//for(j)
		}//for(stage)

		//printf("Step 2 finishes!\n");

		///////////////////////////////////////////////
		//Step3: Doing forekward recursive
		///////////////////////////////////////////////
		//Given a initial state
		float fStates[NUM_STAGE_DIM][NUM_STATE_DIM];
		float fControls[NUM_STAGE_DIM][NUM_STATE_DIM];
		float fOptimalValue;
		for(int i=0; i<nDimState; i++)
			fStates[0][i] = 3.0;

		for(int iStage=0; iStage<nDimStage-1; iStage++)
		{
			CDPNode *pFindNode		= new CDPNode;
			pFindNode->mStage		= iStage;
			pFindNode->mTieIndex	= 1;  //TODO: Correct this if found other ties
			
			for(int i=0; i<nDimState; i++)
				pFindNode->mState[i]	= fStates[iStage][i];

			hash_map<CDPNode, float>::iterator it;
			it = GeneralDPTable.find(*pFindNode);

			if(it != GeneralDPTable.end())
			{
				CDPNode retNode = it->first;
				for(int i=0; i<nDimControl; i++)
					fControls[iStage][i] = retNode.mControl[i];
				CDPNode nextNode;
				DPSystemFunction(retNode, nextNode, 0);
				for(int i=0; i<nDimState; i++)
					fStates[iStage+1][i] = nextNode.mState[i];
				if(iStage == 0)
					fOptimalValue = retNode.fOptimValue;
			}
		}

		/////////////////////
		//Output DP Solutions
		/////////////////////
		//Optimal Tables
		for(int iStage=0; iStage<nDimStage; iStage++)
		{
			printf("\nStep %d Optimal Policy Table:\n", iStage);
			for(int j=0; j<nStateNum; j++)
			{
				CDPNode *pFindNode		= new CDPNode;
				pFindNode->mStage		= iStage;
				pFindNode->mTieIndex	= 1; //TODO: Correct this if found other ties

				for(int i=0; i<nDimState; i++)
					pFindNode->mState[i]	= *(fStateList+i*nStateNum+j);

				for(int i=0; i<nDimState; i++)
					printf("%f\t", pFindNode->mState[i]);

				hash_map<CDPNode, float>::iterator it;
				it = GeneralDPTable.find(*pFindNode);

				if(it != GeneralDPTable.end())
				{
					CDPNode retNode = it->first;
					for(int i=0; i<nDimControl; i++)
						printf("%f\t", retNode.mControl[i]);
					printf("%f\n", -retNode.fOptimValue);
				}        
				else
				{
					for(int i=0; i<nDimControl; i++)
						printf("N.A.\t\t");
					printf("N.A.\n");
				}
			}
		}//for(iStage)

		//Optimal Sequences
		printf("\nOptimal Control Sequences:\n");
		for(int iStage=0; iStage<nDimStage-1; iStage++)
		{
			printf("%d\t", iStage);
			for(int i=0; i<nDimState; i++)
				printf("[S%01d]%f\t", i+1, fStates[iStage][i]);

			for(int i=0; i<nDimControl; i++)
				printf("[U%01d]%f\t", i+1, fControls[iStage][i]);
			printf("\n");
		}

	}

	return nRetCode;
}
