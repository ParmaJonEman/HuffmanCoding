#include "main.h"

Mat originalImage;
const int bitDepth = 8;
const int intensityRange = 256;
vector<int> grayLevels;
string huffLUT[intensityRange];

float entropyCalc(Mat histogram){
    float theoreticalAveLength = 0.0;
    for(int i : grayLevels) {
        theoreticalAveLength -= histogram.at<float>(0, i) * log2(histogram.at<float>(0, i));
    }
    return theoreticalAveLength;
}

float averageLengthCalc(Mat histogram){
    float aveLength = 0.0;
    for(int i : grayLevels) {
        cout << i << ": " << huffLUT[i] << endl;
        aveLength += huffLUT[i].length() * histogram.at<float>(0, i);
    }
    return aveLength;
}


// a huffNode is a special struct that contains a source value, a probability value, and a left and right child
struct huffNode {
    int source;
    float probability;
    huffNode *left, *right;

    huffNode(int source, float probability) {
        left = nullptr;
        right = nullptr;
        this->source = source;
        this->probability = probability;
    }
};

//a huffNode is greater than another huffNode if its probability is greater
struct nodeComparator {
    bool operator()(huffNode *left, huffNode *right) {
        return (left->probability > right->probability);
    }
};

void constructHuffLUT(struct huffNode *node, string code) {
    if (!node) {
        return;
    }

    //checks if it's a combined node, and if it isn't, adds it to the LUT
    if (node->source != -1) {
        huffLUT[node->source] = code;
    }

    constructHuffLUT(node->left, code + "0");

    constructHuffLUT(node->right, code + "1");
}

void constructHuffmanTree(Mat normalizedHistogram) {

    //creates a queue of huffNodes using the huffNode struct created above, as well as the special comparator
    priority_queue<huffNode *, vector<huffNode *>, nodeComparator> huffQueue;

    //puts all source values with non-zero probabilities onto the huffQueue
    for (int i = 0; i < normalizedHistogram.total(); i++)
        if(normalizedHistogram.at<float>(0, i)) {
            huffQueue.push(new huffNode(i, normalizedHistogram.at<float>(0, i)));
            grayLevels.push_back(i);
        }

    struct huffNode *left, *right, *top;
    while (huffQueue.size() != 1) {
        //takes two lowest ranking nodes, stores their values and pops them off the queue
        left = huffQueue.top();
        huffQueue.pop();
        right = huffQueue.top();
        huffQueue.pop();

        //creates a new root node (with value -1 so we know it's not a real source value)
        // with the two previously popped nodes as children, creating a new binary tree
        top = new huffNode(-1, left->probability + right->probability);
        top->left = left;
        top->right = right;

        //then it pushes that new node back onto the queue
        huffQueue.push(top);
    }

    //when there's only one node left, we send it to the LUT constructing function
    constructHuffLUT(huffQueue.top(), "");
}

int main(int argc, char **argv) {
    string imageFile;
    parseParameters(argc, argv, &imageFile);
    cout << "Selected Image: " << imageFile << endl;

    try {
        originalImage = imread(imageFile, 1);
        if (originalImage.empty())
            throw (string("Cannot open input originalImage ") + imageFile);

        int totalPixels = originalImage.total();
        cvtColor(originalImage, originalImage, COLOR_BGR2GRAY);

        int histSize = intensityRange;
        float range[] = {0, intensityRange};
        const float *histRange[] = {range};
        bool uniform = true, accumulate = false;
        Mat hist;
        calcHist(&originalImage, 1, 0, Mat(), hist, 1, &histSize, histRange, uniform, accumulate);
        for (int i = 0; i < hist.rows; i++) {
            hist.at<float>(0, i) = hist.at<float>(0, i) / totalPixels;
        }

        constructHuffmanTree(hist);

        float aveLength = averageLengthCalc(hist);

        float theoreticalAveLength = entropyCalc(hist);

        cout << "Average Code Block Length: " << aveLength << endl;
        cout << "Theoretical Average Code Block Length: " << theoreticalAveLength << endl;
        cout << "Total Original Bits: " << originalImage.total() << endl;
        cout << "Compression Ratio: " <<  (originalImage.total() * bitDepth) / (originalImage.total() * aveLength) << endl;
    }
    catch (string &str) {
        cerr << "Error: " << argv[0] << ": " << str << endl;
        return (1);
    }
    catch (Exception &e) {
        cerr << "Error: " << argv[0] << ": " << e.msg << endl;
        return (1);
    }
    return 0;
}

static int parseParameters(int argc, char **argv, string *imageFile) {
    String keys =
            {
                    "{help h usage ? |                            | print this message   }"
                    "{@imagefile|| originalImage you want to use	}"
            };

    // Get required parameters.
    // If no image is passed in, or if the user passes in a help param, usage info is printed
    CommandLineParser parser(argc, argv, keys);
    parser.about("Huffman v1.0");

    if (!parser.has("@imagefile") || parser.has("help")) {
        parser.printMessage();
        return (0);
    }

    *imageFile = parser.get<string>("@imagefile");
    return (1);
}
