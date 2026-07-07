#include <bits/stdc++.h>

using namespace std;

int main()
{
    ifstream in("/home/stepan/StudioProjects/nogpt/data/data.txt");
    ofstream out("/home/stepan/StudioProjects/nogpt/data/GOODdata", std::ios::binary);

    int V;
    in >> V;
    out.write((const char *)&V, sizeof(V));

    cout << "W" << endl;
    string word;
    for (size_t i = 0; i < V; i++)
    {
        int length;
        in >> word;
        length = word.size();

        out.write((const char *)&length, sizeof(length));
        out.write(word.data(), length);
    }

    int R;
    in >> R;
    out.write((const char *)&R, sizeof(R));

    cout << "R" << endl;
    for (size_t i = 0; i < R; i++)
    {
        int a, b;
        float w;

        in >> a >> b >> w;

        out.write((const char *)&a, sizeof(a));
        out.write((const char *)&b, sizeof(b));
        out.write((const char *)&w, sizeof(w));
    }

    in.close();
    out.close();
}