#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <stdint.h>
#include <string.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <deque>
#include <string>

#include <algorithm>
#include <functional>
#include <bitset>
#include <functional>
#include <chrono>
#include <random>

#define sqr(x) (x) * (x)

typedef long long int i64;

using namespace std;
using namespace std::chrono;

const i64 mod = 1'000'000'000ll + 7;
//const i64 mod = 998'244'353ll;

const i64 inf = 1'000'000'000'000'000'000ll;

const long double eps = 1e-8;

struct TResource {
    i64 Cores;
    i64 Memory;
};

struct TMove {
    i64 OldServerId;
    i64 NewServerId;
    i64 VmId;
};

i64 ServersCount, VmsCount;

vector<TResource> Servers;
vector<TResource> Vms;

vector<i64> Allocation;
vector<i64> Target;

using TStep = vector<TMove>;
vector<TStep> Steps;

void ReadInput() {
    cin >> ServersCount >> VmsCount;

    Servers = vector<TResource>(ServersCount);
    for (i64 i = 0; i < ServersCount; i++) {
        cin >> Servers[i].Cores >> Servers[i].Memory;
    }

    Vms = vector<TResource>(VmsCount);
    for (i64 i = 0; i < VmsCount; i++) {
        cin >> Vms[i].Cores >> Vms[i].Memory;
    }

    Allocation = vector<i64>(VmsCount);
    Target = vector<i64>(VmsCount);

    for (i64 i = 0; i < VmsCount; i++) {
        cin >> Allocation[i] >> Target[i];
    }
}

void WriteOutput() {
    cout << Steps.size() << endl;
    for (auto& step : Steps) {
        cout << step.size() << endl;

        for (auto& move : step) {
            cout << move.OldServerId << " " << move.NewServerId << " " << move.VmId << endl;
        }
    }
}

bool IsEnough(i64 serverId, i64 vmId) {
    return Servers[serverId].Cores >= Vms[vmId].Cores && Servers[serverId].Memory >= Vms[vmId].Memory;
}

void Simlulate(TStep& step) {
    for (auto& move : step) {
        Servers[move.NewServerId].Cores += Vms[move.VmId].Cores;
        Servers[move.NewServerId].Memory += Vms[move.VmId].Memory;

        Servers[move.OldServerId].Cores -= Vms[move.VmId].Cores;
        Servers[move.OldServerId].Memory -= Vms[move.VmId].Memory;

        Allocation[move.VmId] = move.NewServerId;
    }
}

void Rollback(TStep& step) {
    for (auto& move : step) {
        Servers[move.NewServerId].Cores -= Vms[move.VmId].Cores;
        Servers[move.NewServerId].Memory -= Vms[move.VmId].Memory;

        Servers[move.OldServerId].Cores += Vms[move.VmId].Cores;
        Servers[move.OldServerId].Memory += Vms[move.VmId].Memory;

        Allocation[move.VmId] = move.OldServerId;
    }
}



int main(int argc, char* argv[]) {
    ios::sync_with_stdio(0); cin.tie(0); cout.tie(0); cout.precision(15); cout.setf(ios::fixed); cerr.precision(15); cerr.setf(ios::fixed);

    if (sizeof(i64) != sizeof(long long int)) {
        cerr << "i64 != long long int" << endl;
    }

    ReadInput();

    bool moved = true;
    while (moved) {
        moved = false;
        for (i64 i = 0; i < VmsCount; i++) {
            if (Allocation[i] == Target[i]) {
                continue;
            }
            if (!IsEnough(Target[i], i)) {
                continue;
            }
            Steps.push_back(TStep());
            Steps.back().push_back({ Allocation[i], Target[i], i });
            Simlulate(Steps.back());
            moved = true;
            break;
        }
    }

    for (i64 i = 0; i < VmsCount; i++) {
        if (Allocation[i] == Target[i]) {
            continue;
        }
        cerr << "!!!" << i << endl;
    }

    WriteOutput();

    return 0;
}