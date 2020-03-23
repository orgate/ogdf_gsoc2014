/*
 * $Revision: NA $
 * 
 * last checkin:
 *   $Author: alfred $ 
 *   $Date: 2014-03-24 22:44:58 +0100 (Mo, 24 Mar 2014) $ 
 ***************************************************************/
 
/** \file
 * \brief 
 * Source file (programming exercise) written as part 
 * of the proposal to GSoC-2014, for the project:
 * "Preprocessing for Steiner Tree Problems", under OGDF
 * 
 * \author Alfred Ajay Aureate R
 * 
 * \par License:
 * NA
 * 
 * \par
 * This program takes in a .stp file (from SteinLib) and reduces 
 * the graph described in the file based on the following tests
 * as per the user's choice:
 * Test-1: 
 *		A nonterminal node of degree one can be removed.
 * Test-2: 
 *		Every non-terminal with degree 2 can be removed and its 
 * adjacent nodes can be connected by an edge of the total weight 
 * (u–v–w → u–w).
 * Test-3: 
 * 		All parallel edges are converted to single edges.
 * 
 * \par
 * Usage: In Linux terminal, use the following command line:
 * g++ -I/path/to/OGDF/include/ -O2 filename.cpp -o objectname 
 * -L/path/to/OGDF/_release/ -lOGDF -pthread
 * Then to run the object file, use the following command line:
 * ./objectname SteinLibfilename.stp 
 * or the following line:
 * ./objectname SteinLibfilename.stp n
 * The above line runs all the test, but if a user wants to run
 * only some of the tests, then use the following command line:
 * ./objectname SteinLibfilename.stp Y
 * And then answer 'Y' or 'n' for each question about the 
 * reduction tests
 * 
 * \par
 * The reduced graph is written to ReducedGraph.gml 
 * 
 * \see  NA
 ***************************************************************/
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/graph_generators.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/basic/simple_graph_alg.h>
  
using namespace ogdf;

int main (int arg, char* argv[])	{
	int i, j, numberOFIterations;
	i = 0;
	numberOFIterations = 10; 			//!< Number of times all the tests are performed
	double m_edgeDoubleWeight;
	EdgeWeightedGraph<double> EWG;
	List<node> m_nodeList; 				//!< List storing all nodes in the graph.
	List<node> m_deleteNodeList; 		//!< List storing elements to be deleted.
	NodeArray<bool> m_isTerminal(EWG); 	//!< Array to know whether a node is terminal
	node v, m_nodesOfEdge[2], m_deleteNode;
	edge e;
	char c[] = "YYY", charRun;
	if (arg<3)
		charRun = 'n'; 					//!< This means default run with all the tests
	else
		charRun = (*argv[2]);

	//! Stores the atributes of the edge weighted graph
	GraphAttributes GA (EWG, GraphAttributes::edgeGraphics |
							 GraphAttributes::edgeDoubleWeight);
	GA.setDirected(false); 				//!< Graph is taken as undirected.

	//! Reads .stp file
	GraphIO::readSTP(EWG, m_nodeList, m_isTerminal, argv[1]);

	//! If user wants to mention the tests to be run
	if(charRun=='Y') {
		cout << "Test-1: "					<< endl;
		cout << "	A nonterminal node of degree one can be removed." << endl;
		cout << "Do you want to perform this test ? [Y/n] ";
		cin >> c[0];

		cout << "Test-2: "					<< endl;
		cout << "	Every non-terminal with degree 2 can be removed ";
		cout << "and its adjacent nodes can be connected by an edge of ";
		cout << "the total weight (u–v–w → u–w)." << endl;
		cout << "Do you want to perform this test ? [Y/n] ";
		cin >> c[1];

		cout << "Test-3: "					<< endl;
		cout << "	All parallel edges are converted to single edges." << endl;
		cout << "Do you want to perform this test ? [Y/n] ";
		cin >> c[2];

		cout << "Please mention the number of times the tests should ";
		cout << "be run "<<endl;
		cout << "(Default is: 10) ";
		cin >> numberOFIterations;
	}
	cout << "Initially..."				<< endl;
	cout << "Number of nodes     = "	<< EWG.numberOfNodes() << endl;
	cout << "Number of edges     = "	<< EWG.numberOfEdges() << endl;
	cout << "Number of terminals = "	<< m_nodeList.size() << endl;

	//! This loop performs the tests for \a numberOFIterations times
	while (i < numberOFIterations)	{

		//! Stores the non-terminals of degree 1 or 2 into a list
		/* This loop runs for all nodes in the graph and the nodes
		 * which are non-terminals, with degree 1 or 2 are 
		 * stored into a list g_deleteNodeList.
		 */
		forall_nodes(v, EWG)	{
			if (!m_isTerminal[v]) {
				if ((v->degree()==1) & (c[0]=='Y'))
					m_deleteNodeList.pushBack(v);
				else if ((v->degree()==2) & (c[1]=='Y'))
					m_deleteNodeList.pushBack(v);
			}
		}

		//! Deletes nodes in \a m_deleteNodeList from graph \a G
		/* This loop deletes all the nodes in the \a m_deleteNodeList
		 * from the graph, \a G as per the specification of the tests 
		 * (Test-1 and Test-2)
		 * Test-1: 
		 *		A nonterminal node of degree one can be removed.
		 * Test-2: 
		 *		Every non-terminal with degree 2 can be removed and its 
		 * adjacent nodes can be connected by an edge of the total weight 
		 * (u–v–w → u–w).
		 */
		while (m_deleteNodeList.size()!=0)	{
			m_deleteNode = m_deleteNodeList.popFrontRet();			
			if (m_deleteNode->degree()==1)	
				EWG.delNode(m_deleteNode);				
			else if (m_deleteNode->degree()==2)	{
				j = 0;
				m_edgeDoubleWeight = 0.0;
				forall_adj_edges(e, m_deleteNode)  {
					m_edgeDoubleWeight += EWG.weight(e);
					m_nodesOfEdge[j] = e->source()!=m_deleteNode ? e->source() : e->target();
					j++;
				}
				EWG.newEdge(m_nodesOfEdge[0], m_nodesOfEdge[1], m_edgeDoubleWeight);
				EWG.delNode(m_deleteNode);
			}
		}

		//! Parallel edges are made into single undirected edges (Test-3)
		/* Test-3: 
 		 *		All parallel edges are converted to single edges.
		 */
 		  if (c[2]=='Y')
			makeParallelFreeUndirected(EWG);

		//! It assigns all the edge weights to graph attribute, \a GA
		//! so that weights gets mentioned in the .gml file
		forall_edges(e, EWG)	
			GA.doubleWeight(e) = EWG.weight(e);
		i++;
	}

	cout << "Finally for the reduced instance after " << i <<" iterations" << endl;
	cout << "Number of nodes     = " << EWG.numberOfNodes() << endl;
	cout << "Number of edges     = " << EWG.numberOfEdges() << endl;
	cout << "Number of terminals = " << m_nodeList.size()   << endl;

	//! Additional information about some more reductions
	cout << "Further, there are more tests that could also be applied: " << endl;
	cout << "PTm test, NTDk test, NV test, SE test, etc." << endl;
	cout << "For example: PTm test is as follows: " << endl;
	cout << "Every edge (u,v) with c(u,v ) > b(u,v) can be removed from G." << endl;
	cout << "where, c(u,v) is the cost of the edge [u,v] and " << endl;
	cout << "       b(u,v) is the bottle neck distance between u and v" << endl;

	//! Reduced graph is written to a .gml file
	GraphIO::writeGML(GA, "ReducedGraph.gml");
  
	return 0;
}
