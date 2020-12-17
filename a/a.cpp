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

struct TRandom {
    using Type = i64;
    mt19937 Rng;
    uniform_int_distribution<Type> Dist;

    TRandom() : Rng((chrono::steady_clock::now().time_since_epoch().count())), Dist(0ll, std::numeric_limits<Type>::max()) {
    }

    Type GetRandomNumber() {
        return Dist(Rng);
    }
};

struct TResource {
    i64 Cores;
    i64 Memory;
};

struct TMove {
    i64 OldServerId;
    i64 NewServerId;
    i64 VmId;

    bool operator<(const TMove& move) const {
        if (OldServerId < move.OldServerId) {
            return true;
        }
        if (OldServerId > move.OldServerId) {
            return false;
        }

        if (NewServerId < move.NewServerId) {
            return true;
        }
        if (NewServerId > move.NewServerId) {
            return false;
        }

        if (VmId < move.VmId) {
            return true;
        }
        if (VmId > move.VmId) {
            return false;
        }

        return false;
    }
};

i64 ServersCount, VmsCount;

vector<TResource> Servers;
vector<TResource> Vms;

vector<i64> Allocation;
vector<i64> Target;

vector<i64> Bonus;

using TStep = vector<TMove>;
vector<TStep> Steps;

TRandom rng;

void GenerateSample(i64 serversCount, i64 vmsCount, i64 lowVmMemoryLimit, i64 highVmMemoryLimit, i64 lowVmCoresLimit, i64 highVmCoresLimit) {
    vector<TResource> servers(serversCount);
    vector<TResource> targetServers(serversCount);
    vector<TResource> allocationServers(serversCount);

    vector<TResource> vms(vmsCount);
    vector<i64> allocation(vmsCount);
    vector<i64> target(vmsCount);

    for (i64 i = 0; i < vmsCount; i++) {
        allocation[i] = rng.GetRandomNumber() % serversCount;
        target[i] = rng.GetRandomNumber() % serversCount;
        vms[i].Cores = lowVmCoresLimit + rng.GetRandomNumber() % (highVmCoresLimit - lowVmCoresLimit + 1);
        vms[i].Memory = lowVmMemoryLimit + rng.GetRandomNumber() % (highVmMemoryLimit - lowVmMemoryLimit + 1);

        allocationServers[allocation[i]].Cores += vms[i].Cores;
        allocationServers[allocation[i]].Memory += vms[i].Memory;
        targetServers[target[i]].Cores += vms[i].Cores;
        targetServers[target[i]].Memory += vms[i].Memory;
    }

    for (i64 i = 0; i < serversCount; i++) {
        servers[i].Cores = max(allocationServers[i].Cores, targetServers[i].Cores);
        servers[i].Memory = max(allocationServers[i].Memory, targetServers[i].Memory);

        //servers[i].Cores += highVmCoresLimit;
        //servers[i].Memory += highVmMemoryLimit;
    }

    cout << serversCount << " " << vmsCount << endl;
    for (i64 i = 0; i < serversCount; i++) {
        cout << servers[i].Cores << " " << servers[i].Memory << endl;
    }

    for (i64 i = 0; i < vmsCount; i++) {
        cout << vms[i].Cores << " " << vms[i].Memory << endl;
    }

    for (i64 i = 0; i < vmsCount; i++) {
        cout << allocation[i] << " " << target[i] << endl;
    }
}
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

    for (i64 i = 0; i < VmsCount; i++) {
        Servers[Allocation[i]].Cores -= Vms[i].Cores;
        Servers[Allocation[i]].Memory -= Vms[i].Memory;
    }

    Bonus = vector<i64>(ServersCount);
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

i64 CalculateScore() {
    i64 movedMemory = 0;
    for (auto& step : Steps) {
        for (auto& move : step) {
            movedMemory += Vms[move.VmId].Memory;
        }
    }
    return Steps.size() * movedMemory + 1;
}

bool IsEnough(i64 serverId, i64 vmId) {
    return Servers[serverId].Cores >= Vms[vmId].Cores && Servers[serverId].Memory >= Vms[vmId].Memory;
}

void SimulateTransfer(const TMove& move) {
    Servers[move.NewServerId].Cores -= Vms[move.VmId].Cores;
    Servers[move.NewServerId].Memory -= Vms[move.VmId].Memory;

    Allocation[move.VmId] = move.NewServerId;
}

void SimulateTransfer(const TStep& step) {
    for (auto& move : step) {
        SimulateTransfer(move);
    }
}

void SimulateCleanup(const TMove& move) {
    Servers[move.OldServerId].Cores += Vms[move.VmId].Cores;
    Servers[move.OldServerId].Memory += Vms[move.VmId].Memory;
}

void SimulateCleanup(const TStep& step) {
    for (auto& move : step) {
        SimulateCleanup(move);
    }
}


void RollbackTransfer(const TMove& move) {
    Servers[move.NewServerId].Cores += Vms[move.VmId].Cores;
    Servers[move.NewServerId].Memory += Vms[move.VmId].Memory;

    Allocation[move.VmId] = move.OldServerId;
}

void RollbackTransfer(const TStep& step) {
    for (auto& move : step) {
        RollbackTransfer(move);
    }
}


void RollbackCleanup(const TMove& move) {
    Servers[move.OldServerId].Cores -= Vms[move.VmId].Cores;
    Servers[move.OldServerId].Memory -= Vms[move.VmId].Memory;
}

void RollbackCleanup(const TStep& step) {
    for (auto& move : step) {
        RollbackCleanup(move);
    }
}


set<i64> CalculateMissedVms() {
    set<i64> misses;
    for (i64 i = 0; i < VmsCount; i++) {
        if (Allocation[i] == Target[i]) {
            continue;
        }
        misses.insert(i);
    }
    return misses;
}

void TransferGreedly() {
    bool moved = true;
    while (moved) {
        moved = false;

        Steps.push_back(TStep());
        vector<i64> networkUsage(ServersCount);
        for (i64 i = 0; i < VmsCount; i++) {
            if (Allocation[i] == Target[i]) {
                continue;
            }
            if (!IsEnough(Target[i], i)) {
                continue;
            }
            if (networkUsage[Allocation[i]] >= 2) {
                continue;
            }
            if (networkUsage[Target[i]] >= 2) {
                continue;
            }

            Steps.back().push_back({ Allocation[i], Target[i], i });
            networkUsage[Allocation[i]] += 1;
            networkUsage[Target[i]] += 1;
            SimulateTransfer(Steps.back().back());
            moved = true;
        }

        SimulateCleanup(Steps.back());
    }
    Steps.pop_back();
}

i64 minSteps = 0;

void CalculateBonus() {
    for (i64 i = 0; i < VmsCount; i++) {
        if (Target[i] == Allocation[i]) {
            continue;
        }
        Bonus[Target[i]] += 1;
        Bonus[Allocation[i]] += 1;
    }
}

void TransferWeighted() {
    //vector<TResource> spare(ServersCount);
    //for (i64 i = 0; i < VmsCount; i++) {
    //    spare[Allocation[i]].Cores += Vms[i].Cores;
    //    spare[Allocation[i]].Memory += Vms[i].Memory;

    //    spare[Target[i]].Cores -= Vms[i].Cores;
    //    spare[Target[i]].Memory -= Vms[i].Memory;
    //}

    /*
    for (i64 i = 0; i < VmsCount; i++) {
        if (Target[i] == Allocation[i]) {
            continue;
        }
        spare[Target[i]].Cores += 1;
        spare[Allocation[i]].Cores += 1;
    }


    for (i64 i = 0; i < ServersCount; i++) {
        //cerr << spare[i].Cores << " ";
        minSteps = max(minSteps, spare[i].Cores);
    }
    //cerr << endl;
    */

    /*
    for (i64 i = 0; i < ServersCount; i++) {
        cerr << Bonus[i] << " ";
    }
    cerr << endl;
    */

    vector<i64> networkUsage(ServersCount);
    auto getScore = [&](i64 vmId) -> i64 {
        if (!IsEnough(Target[vmId], vmId) || networkUsage[Target[vmId]] >= 2 || networkUsage[Allocation[vmId]] >= 2) {
            return inf;
        }
        //i64 val = spare[Allocation[vmId]].Cores * spare[Target[vmId]].Cores;
        //i64 res = (i64)pow(val, 2 - networkUsage[Target[vmId]]);
        //i64 res = -val * (networkUsage[Target[vmId]] + networkUsage[Allocation[vmId]] == 0 ? 1'000'000 : 1);
        //i64 res = -networkUsage[Target[vmId]];
        i64 val = Bonus[Allocation[vmId]] * Bonus[Target[vmId]];
        return -val;
    };


    bool moved = true;
    while (moved) {
        moved = false;
        Steps.push_back(TStep());

        networkUsage = vector<i64>(ServersCount);
        set<pair<i64, i64>> q;
        for (i64 i = 0; i < VmsCount; i++) {
            if (Target[i] == Allocation[i]) {
                continue;
            }
            q.insert({ getScore(i), i });
        }

        while (!q.empty()) {
            i64 i = q.begin()->second;
            auto score = getScore(i);

            if (q.begin()->first != score) {
                q.erase(q.begin());
                q.insert({ score, i });
                continue;
            }

            if (score == inf) {
                break;
            }

            Steps.back().push_back({ Allocation[i], Target[i], i });
            networkUsage[Allocation[i]] += 1;
            networkUsage[Target[i]] += 1;
            Bonus[Target[i]] -= 1;
            Bonus[Allocation[i]] -= 1;
            SimulateTransfer(Steps.back().back());
            moved = true;
            q.erase(q.begin());
        }

        SimulateCleanup(Steps.back());
    }
    Steps.pop_back();
}

void TransferMissedVms(set<i64>& missedVms) {
    set<i64> missedServers;
    for (auto missedVm : missedVms) {
        missedServers.insert(Target[missedVm]);
    }

    set<i64> spareServers;
    for (i64 i = 0; i < ServersCount; i++) {
        if (missedServers.count(i)) {
            continue;
        }
        spareServers.insert(i);
    }

    bool moved = true;
    while (moved) {
        moved = false;

        Steps.push_back(TStep());
        vector<i64> networkUsage(ServersCount);
        for (i64 i = 0; i < VmsCount; i++) {
            if (Allocation[i] == Target[i]) {
                continue;
            }
            if (spareServers.count(Allocation[i])) {
                continue;
            }
            if (networkUsage[Allocation[i]] >= 2) {
                continue;
            }
            for (auto spareServer : spareServers) {
                if (!IsEnough(spareServer, i)) {
                    continue;
                }
                if (networkUsage[spareServer] >= 2) {
                    continue;
                }

                Steps.back().push_back({ Allocation[i], spareServer, i });
                networkUsage[Allocation[i]] += 1;
                networkUsage[spareServer] += 1;
                SimulateTransfer(Steps.back().back());
                moved = true;
                break;
            }
        }

        SimulateCleanup(Steps.back());
    }
    Steps.pop_back();
}

void CompactTwoSteps(i64 targetStart, i64 missedStart) {
    set<TMove> missedMoves;
    for (i64 i = Steps.size() - 1; i >= missedStart; i--) {
        RollbackCleanup(Steps[i]);
        RollbackTransfer(Steps[i]);
        for (auto& e : Steps.back()) {
            missedMoves.insert(e);
        }
        Steps.pop_back();
    }

    vector<TResource> minResources = Servers;
    for (i64 i = missedStart - 1; i >= targetStart; i--) {
        RollbackCleanup(Steps[i]);

        for (auto& e : Steps[i]) {
            minResources[e.NewServerId].Cores = min(minResources[e.NewServerId].Cores, Servers[e.NewServerId].Cores);
            minResources[e.NewServerId].Memory = min(minResources[e.NewServerId].Memory, Servers[e.NewServerId].Memory);

            minResources[e.OldServerId].Cores = min(minResources[e.OldServerId].Cores, Servers[e.OldServerId].Cores);
            minResources[e.OldServerId].Memory = min(minResources[e.OldServerId].Memory, Servers[e.OldServerId].Memory);
        }

        RollbackTransfer(Steps[i]);
    }

    for (i64 i = targetStart; i < missedStart; i++) {
        vector<i64> networkUsage(ServersCount);
        for (auto& e : Steps[i]) {
            networkUsage[e.NewServerId] += 1;
            networkUsage[e.OldServerId] += 1;
            SimulateTransfer(e);
        }

        auto it = missedMoves.begin();
        while (it != missedMoves.end()) {
            if (networkUsage[it->OldServerId] >= 2) {
                it++;
                continue;
            }
            if (networkUsage[it->NewServerId] >= 2) {
                it++;
                continue;
            }
            if (!IsEnough(it->NewServerId, it->VmId)) {
                it++;
                continue;
            }

            bool enoughResources = minResources[it->NewServerId].Cores >= Vms[it->VmId].Cores && minResources[it->NewServerId].Memory >= Vms[it->VmId].Memory;

            if (!enoughResources) {
                it++;
                continue;
            }

            networkUsage[it->NewServerId] += 1;
            networkUsage[it->OldServerId] += 1;

            SimulateTransfer(*it);
            Steps[i].push_back(*it);
            missedMoves.erase(it++);
        }

        SimulateCleanup(Steps[i]);
    }

    while (missedMoves.size()) {
        Steps.push_back(TStep());
         
        vector<i64> networkUsage(ServersCount);
        auto it = missedMoves.begin();
        while (it != missedMoves.end()) {
            if (networkUsage[it->OldServerId] >= 2) {
                it++;
                continue;
            }
            if (networkUsage[it->NewServerId] >= 2) {
                it++;
                continue;
            }
            if (!IsEnough(it->NewServerId, it->VmId)) {
                it++;
                continue;
            }

            networkUsage[it->NewServerId] += 1;
            networkUsage[it->OldServerId] += 1;

            SimulateTransfer(*it);
            Steps.back().push_back(*it);
            missedMoves.erase(it++);
        }

        SimulateCleanup(Steps.back());
    }
}

i64 EstimateSteps() {
    vector<i64> to(ServersCount);
    vector<i64> fr(ServersCount);

    for (i64 i = 0; i < VmsCount; i++) {
        if (Target[i] == Allocation[i]) {
            continue;
        }
        to[Target[i]] += 1;
        fr[Allocation[i]] += 1;
    }

    i64 R = 0;
    for (i64 i = 0; i < ServersCount; i++) {
        R = max(R, to[i] + fr[i]);
    }

    return R;
}

struct TVmsComparator {
    bool operator() (i64 vm1, i64 vm2)  const {
        auto GetScore = [&](i64 vmId) -> i64 {
            return Vms[vmId].Cores * Vms[vmId].Memory;
        };

        return GetScore(vm1) < GetScore(vm2);
    }
};

void MaximizeMoves() {
    vector<i64> filterdVms;
    for (i64 i = 0; i < VmsCount; i++) {
        if (Allocation[i] == Target[i]) {
            continue;
        }
        if (!IsEnough(Target[i], i)) {
            continue;
        }
        filterdVms.push_back(i);
    }

    vector<vector<i64>> to(ServersCount);
    vector<vector<i64>> from(ServersCount);

    for (auto vmId : filterdVms) {
        to[Target[vmId]].push_back(vmId);
        from[Allocation[vmId]].push_back(vmId);
    }

    for (i64 i = 0; i < ServersCount; i++) {
        sort(to[i].begin(), to[i].end());
        sort(from[i].begin(), from[i].end());
    }

    Steps.push_back(TStep());

    vector<i64> network(ServersCount);

    for (i64 i = 0; i < VmsCount; i++) {
        if (Allocation[i] == Target[i]) {
            continue;
        }
        if (!IsEnough(Target[i], i)) {
            continue;
        }
        if (network[Target[i]] >= 2 || network[Allocation[i]] >= 2) {
            continue;
        }
        if (to[Allocation[i]].size()) {
            continue;
        }
        Steps.back().push_back({ Allocation[i], Target[i], i });
        network[Allocation[i]] += 1;
        network[Target[i]] += 1;
        SimulateTransfer(Steps.back().back());
    }

    for (i64 i = 0; i < VmsCount; i++) {
        if (Allocation[i] == Target[i]) {
            continue;
        }
        if (!IsEnough(Target[i], i)) {
            continue;
        }
        if (network[Target[i]] >= 2 || network[Allocation[i]] >= 2) {
            continue;
        }
        Steps.back().push_back({ Allocation[i], Target[i], i });
        network[Allocation[i]] += 1;
        network[Target[i]] += 1;
        SimulateTransfer(Steps.back().back());
    }

    SimulateCleanup(Steps.back());
}

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(0); cin.tie(0); cout.tie(0); cout.precision(15); cout.setf(ios::fixed); cerr.precision(15); cerr.setf(ios::fixed);

    if (sizeof(i64) != sizeof(long long int)) {
        cerr << "i64 != long long int" << endl;
    }


    //GenerateSample(1'000, 1'000'000, 1, 200, 1, 500);
    if (argc > 1 && argv[1][0] == 'g') {
        GenerateSample(100, 1'000, 1, 200, 1, 500);
        return 0;
    }

    ReadInput();

    i64 firstStepsCnt = 0;

    while (true) {
        MaximizeMoves();
        if (Steps.back().empty()) {
            Steps.pop_back();
            break;
        }
    }

    /*
    cerr << "Steps = " << Steps.size() << endl;
    {
        auto missedVms = CalculateMissedVms();
        cerr << "MissedVmsCount = " << missedVms.size() << endl;
    }
    */

    while (Steps.size() > firstStepsCnt) {
        RollbackCleanup(Steps.back());
        RollbackTransfer(Steps.back());
        Steps.pop_back();
    }

    auto start = high_resolution_clock::now();

    i64 mins = inf;

    i64 bestCnt = inf;
    i64 bestScore = inf;
    vector<TStep> bestSteps;

    i64 cnt = 0;
    while (true) {
        cnt += 1;

        CalculateBonus();

        i64 targetStart = Steps.size();
        TransferWeighted();
        //TransferGreedly();

        auto missedVms = CalculateMissedVms();
        //cerr << "Missing " << missedVms.size() << " vms; " << Steps.size() << " steps" << endl;

        mins = min(mins, (i64)missedVms.size());

        auto current = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(current - start);

        if (duration.count() > 5'000'000ll) {
            break;
        }


        if (missedVms.size() == 0) {
            if (bestScore > CalculateScore()) {
                bestScore = CalculateScore();
                bestSteps = Steps;
            }

            if (missedVms.size() == 0) {
                for (auto& e : Steps.back()) {
                    Bonus[e.NewServerId] += 1;
                    Bonus[e.OldServerId] += 1;
                }
            }
        }
        else if (false) {
            auto BonusBackup = Bonus;
            TransferMissedVms(missedVms);
            CalculateBonus();
            TransferWeighted();

            missedVms = CalculateMissedVms();
            /*
            if (missedVms.size() == 0) {
                if (bestScore > CalculateScore()) {
                    bestScore = CalculateScore();
                    bestSteps = Steps;
                }
            }
            */

            Bonus = BonusBackup;
        }

        while (Steps.size() > firstStepsCnt) {
            RollbackCleanup(Steps.back());
            RollbackTransfer(Steps.back());
            Steps.pop_back();
        }
    }

    Steps = bestSteps;

    auto missedVms = CalculateMissedVms();
    cerr << "Missing " << missedVms.size() << " vms; " << Steps.size() << " steps" << endl;

    for (auto& e : Steps) {
        cerr << e.size() << endl;
    }

    if (argc > 1 && argv[1][0] == 's') {
        for (i64 i = 0; i < VmsCount; i++) {
            if (Target[i] != Allocation[i]) {
                cerr << "Missmatch for VM # " << i << "; " << " bestSteps = " << bestCnt << endl;
                return 0;
            }
        }
        i64 R = CalculateScore();
        cerr << R << " = " << Steps.size() << " * " << R / Steps.size() << "; " << cnt << " check " << endl;
        return 0;
    }

    WriteOutput();
    return 0;
}