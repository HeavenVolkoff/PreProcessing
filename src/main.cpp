#include "../include/PreProcessing.h"

using namespace std;
using namespace voicer;

/**
 * Log received vector
 * @param v
 */
template<class T>
void printVector(const vector<T> &v, size_t limit = 100) {
    string arrStr = "[";

    for (int i = 0; i < v.size() && i <= limit; i++) {
        arrStr += to_string(v[i]) + (i == v.size() - 1 ? "" : i == limit ? "..." : ", ");
    }

    arrStr += "]";

    cout << arrStr << endl;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "Usage" <<  argv[0] << ": <WAV file>" << endl;
        return -1;
    }

    PreProcessing p;
    vector<vector<double>> result = p.loadAudioFile(argv[1]);

    for(int j = 0; j < result.size(); j++){
        printVector(result[j]);
    }

    return 0;
}
