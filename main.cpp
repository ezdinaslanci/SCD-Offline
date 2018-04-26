#include "DetectionTools.h"
#include "linked_list.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/filereadstream.h"
#include <boost/filesystem.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/tokenizer.hpp>
#include <pthread.h>

// namespaces
using namespace std;
using namespace arma;
using namespace boost::filesystem;
using namespace rapidjson;

// globals
pthread_mutex_t mutex0;
int depth, movingWindowSize, lookBuffer, lookSide, minPeakDistance, startPoint, normalizeZerothOrders, numberOfDependenciesConsidered;
double lambda, minPeakHeight, discountFactor;
char markovDependencySwitch[3];

// synonyms
typedef struct experiments {
    char fName[500];
    char resultDir[500];
	int id = 0;
    struct experiments * next;
} experiments;
typedef experiments *Experiments;

// function prototypes
void *main_task (void);
void readSettings ();
void displaySettings ();

void *main_task(void *arg){
		string inputSequenceFileName(((Experiments)arg)->fName);
		string resultsPath(((Experiments)arg)->resultDir);
		int id = ((Experiments)arg)->id;
		// create directory for current experiment
		boost::char_separator <char> sep("/");
		boost::tokenizer <boost::char_separator <char>> tokens(inputSequenceFileName, sep);
		string expName;
		
		for (const auto& token : tokens) {
			expName = token;
		}
		
		expName = expName.substr(0, expName.size() - 4);
		path currentResultFolder(resultsPath + "/" + expName);
		create_directory(currentResultFolder);
		// run SCD
		cout << "\n--- " << expName << " ---\n";
		cout <<id << "\t< SLWEVOMC Probability Estimator >\n";
		mat cm0 = SLWEVOMCProbabilityEstimator(inputSequenceFileName, lambda, depth, markovDependencySwitch, normalizeZerothOrders);

		cout <<id << "\t< Dynamism Amplifier >\n";
		mat dyna = dynamismAmplifier(cm0, lookBuffer, lookSide, startPoint, discountFactor);
		
		cout <<id << "\t< Average of Largest N >\n";
		mat avgOfN = avgOfLargestN(dyna, numberOfDependenciesConsidered);
		
		cout <<id << "\t< Moving Average >\n";
		mat sm = movingMean(avgOfN, movingWindowSize);
        sm = sm/max(max(sm));

		cout <<id << "\t< Peak Analysis >\n";
		mat changePoints = findPeaks(sm, startPoint, minPeakDistance, minPeakHeight);
		changePoints -= round(lookBuffer*1);

		cout <<id << "\t< Logging results... >\n";
		// writeMatrixToFile(cm0, currentResultFolder.string() + "/cm0.txt", '\t', 50);
		// writeMatrixToFile(dyna, currentResultFolder.string() + "/dyna.txt", '\t', 50);
		// writeMatrixToFile(sm, currentResultFolder.string() + "/sm.txt", '\t', 50);
		// writeMatrixToFile(avgOfN, currentResultFolder.string() + "/AOL.txt", '\t', 50);
		// writeMatrixToFile(changePoints, currentResultFolder.string() + "/changePoints.txt", '\t', 0);
		pthread_mutex_lock(&mutex0);
		writeChangePoints("../result2.txt", expName, changePoints, startPoint);
		pthread_mutex_unlock(&mutex0);
	
}

// this function reads settings from a JSON file
void readSettings () {
	
	// reading & parsing JSON parameter file
	FILE* settingsFile = fopen("../parameters.json", "rb");
	char buffer[65536];
	FileReadStream is(settingsFile, buffer, sizeof(buffer));
	Document settingsJSON;
	settingsJSON.ParseStream<0, UTF8<>, FileReadStream>(is);
	
	// checking if the JSON file has any missing parameters
	assert(settingsJSON.HasMember("parameters"));
	assert(settingsJSON["parameters"].HasMember("depth"));
	assert(settingsJSON["parameters"].HasMember("lambda"));
	assert(settingsJSON["parameters"].HasMember("movingWindowSize"));
	assert(settingsJSON["parameters"].HasMember("lookBuffer"));
	assert(settingsJSON["parameters"].HasMember("lookSide"));
	assert(settingsJSON["parameters"].HasMember("minPeakDistance"));
	assert(settingsJSON["parameters"].HasMember("minPeakHeight"));
	assert(settingsJSON["parameters"].HasMember("startPoint"));
	assert(settingsJSON["parameters"].HasMember("markovDependencySwitch"));
	assert(settingsJSON["parameters"].HasMember("normalizeZerothOrders"));
	assert(settingsJSON["parameters"].HasMember("numberOfDependenciesConsidered"));
	assert(settingsJSON["parameters"].HasMember("discountFactor"));
	
	// assigning values
	depth = settingsJSON["parameters"]["depth"].GetInt();
	lambda = settingsJSON["parameters"]["lambda"].GetDouble();
	movingWindowSize = settingsJSON["parameters"]["movingWindowSize"].GetInt();
	lookBuffer = settingsJSON["parameters"]["lookBuffer"].GetInt();
	lookSide = settingsJSON["parameters"]["lookSide"].GetInt();
	minPeakDistance = settingsJSON["parameters"]["minPeakDistance"].GetInt();
	minPeakHeight = settingsJSON["parameters"]["minPeakHeight"].GetDouble();
	startPoint = settingsJSON["parameters"]["startPoint"].GetInt();
	stpcpy(markovDependencySwitch, settingsJSON["parameters"]["markovDependencySwitch"].GetString());
	normalizeZerothOrders = settingsJSON["parameters"]["normalizeZerothOrders"].GetInt();
	numberOfDependenciesConsidered = settingsJSON["parameters"]["numberOfDependenciesConsidered"].GetInt();
	discountFactor = settingsJSON["parameters"]["discountFactor"].GetDouble();
	
}

// this function displays settings read from a JSON file
void displaySettings () {
	
	cout << "Parameters: \n\t" << 
			"depth: " << depth << "\n\t" <<
			"lambda: " << lambda << "\n\t" <<
			"movingWindowSize: " << movingWindowSize << "\n\t" <<
			"lookBuffer: " << lookBuffer << "\n\t" <<
			"lookSide: " << lookSide << "\n\t" <<
			"minPeakDistance: " << minPeakDistance << "\n\t" <<
			"minPeakHeight: " << minPeakHeight << "\n\t" <<
			"startPoint: " << startPoint << "\n\t" <<
			"markovDependencySwitch: " << markovDependencySwitch << "\n\t" <<
			"normalizeZerothOrders: " << normalizeZerothOrders << "\n\t" <<
			"numberOfDependenciesConsidered: " << numberOfDependenciesConsidered << "\n\t" <<
			"discountFactor: " << discountFactor << "\n\t" <<
			endl;
			
}

int main(int argc, char* argv[]) {
	
	readSettings();
	
	Linked_List Experiments_list = create_linked_list();
    boost::filesystem::ofstream output("../result2.txt");
	
    // create results directory
    path currentPath(current_path());
    path parentPath = currentPath.parent_path();
	path resultsPath(parentPath.string() + "/results");
    if (create_directory(resultsPath)) {
        cout << "\n" << "Result Directory Created: " << resultsPath << endl;
    }
	
    // directory that contains input files
    path inputSequencesPath(parentPath.string() + "/inputSequences");

    // get the number of input sequences
    long numberOfInputSequences = count_if(directory_iterator(inputSequencesPath), directory_iterator(), static_cast <bool(*)(const path&)> (is_regular_file) );
    
    cout << "\nBulk experiment started with " << numberOfInputSequences << " input sequences.\n";

	//thread id's
    pthread_t TaskT [numberOfInputSequences];
	pthread_mutex_init(&mutex0,NULL);


    // loop that gets files
	
    if (is_directory(inputSequencesPath)) {
		int i = 0;
        for (auto& inputSequenceFileName : boost::make_iterator_range(directory_iterator(inputSequencesPath), {})) {
			Experiments temp = (Experiments)malloc(sizeof(experiments));
			strcpy(temp -> fName, inputSequenceFileName.path().string().c_str());
			strcpy(temp -> resultDir, resultsPath.string().c_str());
			temp -> id = ++i;
			add_toHead(Experiments_list,temp);
        }
    }

	Linked_List_Node iterator = Experiments_list->head;
	for(int i = 0; i < numberOfInputSequences; i++){
		pthread_create(&TaskT[i],NULL,main_task,iterator->data);
		iterator = iterator -> next;
	}
	
	for(int i = 0; i < numberOfInputSequences; i++){
		pthread_join(TaskT[i],NULL);
	}
    
    return 0;

}
