/*
 * IOGTest.cpp
 *
 *  Created on: 12.12.2012
 *      Author: Christian Staudt (christian.staudt@kit.edu)
 */

#ifndef NOGTEST

#include "IOGTest.h"

#include <fstream>
#include <unordered_set>
#include <vector>

#include "../FastMETISGraphReader.h"
#include "../METISGraphReader.h"
#include "../METISGraphWriter.h"
#include "../PartitionWriter.h"
#include "../PartitionReader.h"
#include "../GraphIO.h"
#include "../DotGraphWriter.h"
#include "../DGSReader.h"
#include "../EdgeListIO.h"
#include "../EdgeListPartitionReader.h"
#include "../SNAPEdgeListPartitionReader.h"
#include "../SNAPGraphWriter.h"
#include "../FastMETISGraphReader.h"

#include "../../community/GraphClusteringTools.h"
#include "../../auxiliary/Log.h"
#include "../../graph/GraphGenerator.h"
#include "../../community/ClusteringGenerator.h"
#include "../../structures/Partition.h"
#include "../../community/Modularity.h"
#include "../../community/PLP.h"


namespace NetworKit {

TEST_F(IOGTest, testGraphIOEdgeList) {
	GraphGenerator graphGen;
	Graph G = graphGen.makeCircularGraph(20);
	GraphIO graphio;
	std::string path = "output/edgelist.txt";
	graphio.writeEdgeList(G, path);

	bool exists = false;
	std::ifstream file(path);
	if (file) {
		exists = true;
	}
	EXPECT_TRUE(exists) << "A file should have been created : " << path;
}

TEST_F(IOGTest, testGraphIOAdjacencyList) {
	GraphGenerator graphGen;
	Graph G = graphGen.makeCircularGraph(20);
	GraphIO graphio;
	std::string path = "output/circular.adjlist";
	graphio.writeAdjacencyList(G, path);

	bool exists = false;
	std::ifstream file(path);
	if (file) {
		exists = true;
	}
	EXPECT_TRUE(exists) << "A file should have been created : " << path;
}


TEST_F(IOGTest, testGraphIOForIsolatedNodes) {
	GraphGenerator graphGen;
	Graph G(20);
	GraphIO graphio;
	std::string path = "output/isolated.adjlist";
	graphio.writeAdjacencyList(G, path);

	bool exists = false;
	std::ifstream file(path);
	if (file) {
		exists = true;
	}
	EXPECT_TRUE(exists) << "A file should have been created : " << path;
}



TEST_F(IOGTest, testMETISGraphReader) {
	std::string path = "input/jazz.graph";

	METISGraphReader reader;
	Graph G = reader.read(path);
	count n = 198;
	count m = 2742;
	EXPECT_FALSE(G.isEmpty());
	EXPECT_EQ(n, G.numberOfNodes()) << "There are " << n << " nodes in the  graph";
	EXPECT_EQ(m, G.numberOfEdges()) << "There are " << m << " edges in the  graph";

	for (index v = 0; v < n; ++v) {
		EXPECT_TRUE(G.hasNode(v)) << "Node " << v << " should be there";
	}


	// graph polblogs (has singletons)
	path = "input/polblogs.graph";
	G = reader.read(path);
	n = 1490;
	m = 16715;
	EXPECT_FALSE(G.isEmpty());
	EXPECT_EQ(n, G.numberOfNodes()) << "There are " << n << " nodes in the  graph";
	EXPECT_EQ(m, G.numberOfEdges()) << "There are " << m << " edges in the  graph";

	for (index v = 0; v < n; ++v) {
		EXPECT_TRUE(G.hasNode(v)) << "Node " << v << " should be there";
	}


	// graph PGPgiantcompo
	path = "input/PGPgiantcompo.graph";
	G = reader.read(path);
	n = 10680;
	m = 24316;
	EXPECT_FALSE(G.isEmpty());
	EXPECT_EQ(n, G.numberOfNodes()) << "There are " << n << " nodes in the  graph";
	EXPECT_EQ(m, G.numberOfEdges()) << "There are " << m << " edges in the  graph";

	for (index v = 0; v < n; ++v) {
		EXPECT_TRUE(G.hasNode(v)) << "Node " << v << " should be there";
	}
}

TEST_F(IOGTest, testMETISGraphReaderWithWeights) {
	std::string path = "input/lesmis.graph";

	METISGraphReader reader;
	Graph G = reader.read(path);

	EXPECT_FALSE(G.isEmpty());
	count n = 77;
	count m = 254;
	EXPECT_EQ(n, G.numberOfNodes()) << "There are " << n << " nodes in the  graph";
	EXPECT_EQ(m, G.numberOfEdges()) << "There are " << m << " edges in the  graph";

	for (index v = 0; v < n; ++v) {
		EXPECT_TRUE(G.hasNode(v)) << "Node " << v << " should be there";
	}
}

TEST_F(IOGTest, testMETISGraphWriter) {
	std::string path = "input/jazz1.graph";
	Graph G = Graph(3);
	G.addEdge(0,2);
	G.addEdge(1,1);
	G.addEdge(1,2);
	G.addEdge(2,2);

	METISGraphWriter writer;
	writer.write(G, false, path);
    bool exists = false;
	std::ifstream file(path);
	if (file) {
		exists = true;
	}
	EXPECT_TRUE(exists) << "A file should have been created : " << path;

}


TEST_F(IOGTest, testMETISGraphWriterWithWeights) {
	std::string path = "input/jazz2.graph";
	Graph G = Graph(5);
	G.addEdge(0,2);
	G.addEdge(0,1);
	G.addEdge(0,0);
	G.addEdge(1,1);

	METISGraphWriter writer;
	writer.write(G, true, path);
    bool exists = false;
	std::ifstream file(path);
	if (file) {
		exists = true;
	}
	EXPECT_TRUE(exists) << "A file should have been created : " << path;

}

TEST_F(IOGTest, testMETISGraphReaderWithDoubleWeights) {
	std::string path = "input/jazz2double.graph";

	FastMETISGraphReader reader;
	Graph G = reader.read(path);

	EXPECT_FALSE(G.isEmpty());
	count n = 5;
	count m = 6;
	EXPECT_EQ(n, G.numberOfNodes()) << "There are " << n << " nodes in the  graph";
	EXPECT_EQ(m, G.numberOfEdges()) << "There are " << m << " edges in the  graph";

	for (index v = 0; v < n; ++v) {
		EXPECT_TRUE(G.hasNode(v)) << "Node " << v << " should be there";
	}
	double edgeweight = 7.71099;
	double abs = 1e-9;
	EXPECT_LE(G.totalEdgeWeight()-edgeweight,abs) << "Total edgeweight should be " << edgeweight;
}


TEST_F(IOGTest, testPartitionWriterAndReader) {
	// write clustering first
	std::string path = "output/example.clust";

	GraphGenerator graphGen;
	count n = 100;
	count k = 3;
	Graph G = graphGen.makeCompleteGraph(n);

	ClusteringGenerator clusteringGen;
	Partition zeta = clusteringGen.makeRandomClustering(G, k);

	PartitionWriter writer;
	writer.write(zeta, path);

	// check if file exists
	bool exists = false;
	std::ifstream file(path);
	if (file) {
		exists = true;
	}
	EXPECT_TRUE(exists) << "clustering file should have been written to: " << path;


	PartitionReader reader;
	Partition read = reader.read(path);

	EXPECT_EQ(n, read.numberOfElements()) << "read clustering should contain n nodes";
	EXPECT_TRUE(GraphClusteringTools::isProperClustering(G, read)) << "read clustering should be proper clustering of G";
	EXPECT_TRUE(GraphClusteringTools::equalClusterings(read, zeta, G)) << "read clustering should be identical to created clustering";
}


TEST_F(IOGTest, testDotGraphWriter) {
	GraphGenerator graphGen;
	Graph G = graphGen.makeCompleteGraph(42);

	std::string path = "output/example.dot";

	DotGraphWriter writer;
	writer.write(G, path);

	// check if file exists
	bool exists = false;
	std::ifstream file(path);
	if (file) {
		exists = true;
	}
	EXPECT_TRUE(exists) << "graph file should have been written to: " << path;
}

TEST_F(IOGTest, tryDGSReaderOnBigFile) {
	// read example graph
	DGSReader reader;
	Graph G;
	GraphEventProxy Gproxy(G);
	reader.read("/Users/forigem/KIT/NetworKit-CommunityDetection/input/AuthorsGraph.dgs", Gproxy);
}



TEST_F(IOGTest, tryDGSReader) {
	// read example graph
	DGSReader reader;
	Graph G;
	GraphEventProxy Gproxy(G);
	reader.read("input/example2.dgs", Gproxy);

	// get input parameters
	count nodeCount = G.numberOfNodes();
	DEBUG("Number of nodes " , nodeCount);
	EXPECT_EQ(3, nodeCount);
	count edgeCount = G.numberOfEdges();
	DEBUG("Number of edges " , edgeCount);
	EXPECT_EQ(2, edgeCount);

	G.forNodes([&](node n) {
		DEBUG("DEGREE OF NODE: " , G.degree(n) , "\n");
	});

}

TEST_F(IOGTest, testEdgeListIO) {
	EdgeListIO reader('\t', 1);

	std::string path = "input/LFR-generator-example/network.dat";
	DEBUG("reading file: " , path);
	Graph G = reader.read(path);
	EXPECT_EQ(10, G.numberOfNodes());
	EXPECT_EQ(10, G.numberOfEdges());
	EXPECT_TRUE(G.hasEdge(0, 5));
	EXPECT_TRUE(G.hasEdge(2, 9));
	EXPECT_TRUE(G.hasEdge(1, 7));

	path = "input/example.edgelist";
	DEBUG("reading file: " , path);
	EdgeListIO reader2('\t', 1);
	Graph G2 = reader2.read(path);
	EXPECT_EQ(10, G2.numberOfEdges());
	EXPECT_TRUE(G2.hasEdge(0, 4));

	path = "input/spaceseparated.edgelist";
	DEBUG("reading file: " , path);
	EdgeListIO reader3(' ', 1);
	Graph G3 = reader3.read(path);
	EXPECT_EQ(10, G3.numberOfEdges());
	EXPECT_TRUE(G3.hasEdge(0, 4));

	path = "input/comments.edgelist";
	DEBUG("reading file: " , path);
	EdgeListIO reader4('\t', 1);
	Graph G4 = reader4.read(path);
	EXPECT_EQ(10, G4.numberOfEdges());
	EXPECT_TRUE(G4.hasEdge(0, 4));

}

TEST_F(IOGTest, testEdgeListPartitionReader) {
	EdgeListPartitionReader reader(1);

	Partition zeta = reader.read("input/LFR-generator-example/community.dat");
	//EXPECT_EQ(10, zeta.size());
	EXPECT_EQ(1, zeta[0]);
	EXPECT_EQ(3, zeta[1]);
	EXPECT_EQ(2, zeta[2]);
	EXPECT_EQ(10, zeta.numberOfElements());

}




TEST_F(IOGTest, testMETISGraphReaderForNodeExistence2) {
	METISGraphReader reader;
	Graph G = reader.read("input/jazz.graph");
	EXPECT_TRUE(G.hasNode(0));
	EXPECT_EQ(198, G.numberOfNodes());
	EXPECT_EQ(2742, G.numberOfEdges());
}


TEST_F(IOGTest, tryMETISGraphReaderWithIsolatedNodes) {
	METISGraphReader reader;
	Graph G = reader.read("input/example.graph");
	EXPECT_EQ(4, G.numberOfNodes());
	EXPECT_EQ(2, G.numberOfEdges());
	EXPECT_TRUE(G.hasNode(0));
	EXPECT_TRUE(G.hasNode(1));
	EXPECT_TRUE(G.hasNode(2));
	EXPECT_TRUE(G.hasNode(3));
}


TEST_F(IOGTest, tryReadingLFR) {
	std::string graphPath;
	std::string clustPath;

	std::cout << "[INPUT] LFR graph file path >" << std::endl;
	std::getline(std::cin, graphPath);

	std::cout << "[INPUT] clustering file path >" << std::endl;
	std::getline(std::cin, clustPath);

	EdgeListIO graphReader('\t',1);
	EdgeListPartitionReader clusteringReader;

	Graph G = graphReader.read(graphPath);
	Partition truth = clusteringReader.read(clustPath);

	PLP PLP;
	Partition zeta = PLP.run(G);

	Modularity mod;
	INFO("static clustering quality: " , mod.getQuality(zeta, G));
	INFO("static clustering number of clusters: " , zeta.numberOfSubsets());
	INFO("ground truth quality: " , mod.getQuality(truth, G));
	INFO("ground truth number of clusters: " , truth.numberOfSubsets());

}


TEST_F(IOGTest, tryReadingSNAP) {
	std::string graphPath;

	std::cout << "[INPUT] SNAP graph file path >" << std::endl;
	std::getline(std::cin, graphPath);

	EdgeListIO graphReader(' ', 1);

	Graph G = graphReader.read(graphPath);

	INFO("n = " , G.numberOfNodes());
	INFO("m = " , G.numberOfEdges());

}

TEST_F(IOGTest, trySNAPEdgeListPartitionReader) {
	std::string graphPath;

	std::cout << "[INPUT] SNAP clustering graph file path >" << std::endl;
	std::getline(std::cin, graphPath);


	SNAPEdgeListPartitionReader reader;

	std::vector<std::set<node>> clusterings = reader.read(graphPath);
	INFO("Number of clusters: " , clusterings.size());

}


TEST_F(IOGTest, testSNAPGraphWriter) {
	METISGraphReader reader;
	Graph G = reader.read("input/jazz.graph");

	SNAPGraphWriter writer;
	writer.write(G, "output/SNAPGraphWriter.gr");
}



TEST_F(IOGTest, testFastMETISGraphReader) {
	FastMETISGraphReader reader;
	Graph G = reader.read("input/PGPgiantcompo.graph");

	EXPECT_EQ(10680, G.numberOfNodes());
	EXPECT_EQ(24316, G.numberOfEdges());

	Graph W = reader.read("input/lesmis.graph");

	EXPECT_EQ(77, W.numberOfNodes());
	EXPECT_EQ(254, W.numberOfEdges());
}


TEST_F(IOGTest, tryReadingMETISFile) {
	std::string graphPath;
	std::cout << "[INPUT] graph file path >" << std::endl;
	std::getline(std::cin, graphPath);

	METISGraphReader reader;
	Graph G = reader.read(graphPath);

	EXPECT_TRUE(true);
}


} /* namespace NetworKit */

#endif /* NOGTEST */