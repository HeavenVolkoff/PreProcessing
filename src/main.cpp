#include "../include/PreProcessing.h"

using namespace std;
using namespace motrix;

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

    PreProcessing p([](const vector<double> &frame){
        printVector(frame);
    });

    p.loadAudioFile(argv[1]);

    return 0;
}
