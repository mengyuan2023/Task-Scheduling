#include <iostream>
#include <algorithm>
#include <stack>
#include <vector>
#include <list>
#include <iterator>
#include <chrono>
#include <iomanip>
using namespace std;

void primary_assignment(int t_l_k[][3],int C[], int t_re, int n, int k){
    int t_l_min[n];
    for(int i=0; i<n; i++){
        t_l_min[i] = 0;
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<k; j++){
            if(t_l_min[i]>t_l_k[i][j]){
                t_l_min[i] = t_l_k[i][j];
            }
        }
    }
    //classify tasks: value of C[i] 1:local 0:cloud
    for(int i=0; i<n; i++){
        if(t_l_min[i] > t_re){
            C[i] = 1;
        }
        else
            C[i] = 0;
    }
}

void task_prioritzing(int t_l_k[][3], int pri[], int pri_n[], int g_succ[][10], int w[], int C[], int t_re, int n, int k){
    for(int i=0; i<n; i++){
        if(C[i] == 1){
            w[i] = t_re;
        }
        else{
            int sum = 0;
            for(int j=0; j<k; j++){
                sum += t_l_k[i][j];
                //cout<<t_l_k[j][i]<<endl;
            }
            w[i] = sum/k;
            //cout<<sum<<endl;
        }
    }
    //caculate average of t_l_k of i
    //cout<<t_l_k[2][9];
    pri[n-1] = w[n-1];
    for(int i=n-1; i>=0; i--){
        int max_j = 0;
        for(int j=n-1; j>=0; j--){
            if(g_succ[i][j] == 1 && pri[j] > max_j){
                max_j = pri[j];
            }
        }
        pri[i] = w[i] + max_j;
    }
    //caculate priority
    vector<pair<int,int>> vect;
    for (int i=0; i<n; i++){
        vect.push_back(make_pair(pri[i],i));
    }
    sort(vect.begin(), vect.end());
    for(int i=0; i<n; i++){
        pri_n[i] = vect[i].second;
    }
    //sort according to priority
}

void execution_unit_selection(int t_l_k[][3],int pri_n[], int g_succ[][10], int C[], int rt_l[], int rt_c[], int rt_ws[], int ft_ws[], int ft_wr[], int ft_l[], int ft[], int core[], int core1[], int n, int k, int t_s, int t_r, int t_c, vector<vector<int>> clist){
    int f_i = pri_n[n-1];//schedule first element
    rt_l[f_i] = 0;
    rt_ws[f_i] = 0;
    ft_ws[f_i] = rt_ws[f_i] + t_s;
    rt_c[f_i] = ft_ws[f_i];
    if(C[f_i] == 1){
        ft_wr[f_i] = rt_c[f_i] + t_c + t_r;
        ft_l[f_i] = 0;
        ft[f_i] = ft_wr[f_i];
        core[3] = ft[f_i];
        clist[0].push_back(f_i);
        core1[f_i] = 3;
    }
    else{
        int t_l_min = INT_MAX;
        int index;
        for(int i=0; i<k; i++){
            if(t_l_k[f_i][i]<t_l_min){
                t_l_min = t_l_k[f_i][i];
                index = i;
            }
        }
        ft_l[f_i] = rt_l[f_i] + t_l_min;
        ft_wr[f_i] = rt_c[f_i] + t_c + t_r;
        if(ft_l[f_i] <= ft_wr[f_i]){
            ft[f_i] = ft_l[f_i];
            ft_wr[f_i] = 0;
            core[3] = ft_ws[f_i];
            clist[index+1].push_back(f_i);
            core1[f_i] = index;
        }
        else{
            ft[f_i] = ft_wr[f_i];
            ft_l[f_i] = 0;
            core[index] = ft[f_i];
            clist[0].push_back(f_i);
            core1[f_i] = 3;
        }
    }
    //schedule rest of the elements
    for(int a=n-2; a>=0; a--){
        int i = pri_n[a];
        //cout<<i;
        int max_j_l = 0;
        for(int j=0; j<n; j++){
            if(g_succ[j][i] == 1 && max_j_l < max(ft_l[j],ft_wr[j])){
                max_j_l = max(ft_l[j],ft_wr[j]);
            }
        }
        rt_l[i] = max_j_l;
        int max_j_ws = 0;
        for(int j=0; j<n; j++){
            if(g_succ[j][i] == 1 && max_j_ws < max(ft_l[j],ft_ws[j])){
                max_j_ws = max(ft_l[j],ft_ws[j]);
            }
        }
        rt_ws[i] = max_j_ws;
        ft_ws[i] = max(core[3],rt_ws[i]) + t_s;
        int max_j_c = 0;
        for(int j=0; j<n; j++){
            if(g_succ[j][i] == 1 && max_j_c < ft_wr[j]-t_r){
                max_j_c = ft_wr[j]-t_r;
            }
        }
        rt_c[i] = max(ft_ws[i],max_j_c);
        if(C[i] == 1){
            ft_wr[i] = rt_c[i] + t_c + t_r;
            ft[i] = ft_wr[i];
            ft_l[i] = 0;
            core[3] = ft_ws[i];
            clist[0].push_back(i);
            core1[i] = 3;
        }
        else{
            int rt, index;
            int f = INT_MAX;
            for(int j=0; j<k; j++){
                rt = max(rt_l[i],core[j]);
                if(f > rt + t_l_k[i][j]){
                    f = rt + t_l_k[i][j];
                    index = j;
                }
            }
            rt_l[i] = f - t_l_k[i][index];
            ft_l[i] = f;
            ft_wr[i] = rt_c[i] + t_c + t_r;
            if(ft_l[i] <= ft_wr[i]){
                ft[i] = ft_l[i];
                ft_wr[i] = 0;
                core[index] = ft[i];
                clist[index+1].push_back(i);
                core1[i] = index;
            }
            else{
                ft[i] = ft_wr[i];
                ft_l[i] = 0;
                core[3] = ft[i];
                clist[0].push_back(i);
                core1[i] = 3;
            }
        }
        //cout<<core[2]<<endl;
    }
}

int kernel( vector<vector<int>> clist, int t_l_k[][3], int g_succ[][10],int core1[], int tmax, int t_total, float e_total, int n, int k, int st[], int et[], int E_c, int E_l[][3]){
    int out = 0;
    int count = 0;
    while(out == 0){
        float max_ratio = 0;
        int new_n = 0, new_k = 0, new_index1 = 0, new_index2 = 0, new_t = t_total;
        float new_e = e_total;
        int less_t1 =0, less_t2 = 0;
        int temp_core[10], new_st[10], new_ft[10];;
        for(int i=0; i<n; i++){
            for(int j=0; j<k+1; j++){
                int core2[10], core3[4], rt[10], rt1[10], ft[10], ft1[10], pushed[10];
                vector<vector<int>> tlist(4);
                int index1, index2 = 0;
                for(int i=0; i<10; i++){
                    rt[i] = 0;
                    ft[i] = 0;
                    core2[i] = core1[i];
                    ft1[i] = et[i];
                    rt1[i] = st[i];
                    pushed[i] = 0;
                }
                for(int a=0; a<clist.size(); a++){
                    //core2[i] = core1[i];
                    core3[a] = 0;
                    for(int b=0; b<clist[a].size(); b++){
                        tlist[a].push_back(clist[a][b]);
                    }
                }
                int current_core = core1[i];
                for(int a=0; a<tlist[current_core].size(); a++){
                    if(tlist[current_core][a] == i){
                        index1 = a;
                    }
                }
                tlist[current_core].erase(tlist[current_core].begin()+index1);
                //caculate the ready time of target task
                if(j == 3){
                    int max_j_ws = 0;
                    for(int a=0; a<n; a++){
                        if(g_succ[a][i] == 1 && max_j_ws < ft1[a]){
                            max_j_ws = ft1[a];
                        }
                    }
                    rt[i] = max_j_ws;
                }
                else{
                    int max_j_l = 0;
                    for(int a=0; a<n; a++){
                        if(g_succ[a][i] == 1 && max_j_l < ft1[a]){
                            max_j_l = ft1[a];
                        }
                    }
                    rt[i] = max_j_l;
                }
                core2[i] = j;
                //cout<<tlist[j].size()<<" "<<count<<endl;
                if(tlist[j].size() == 0){
                    index2 = 0;
                }
                else if(tlist[j].size() == 1){
                    if(rt1[tlist[j][0]] > rt[i]){
                        index2 = 0;
                    }
                    else{
                        index2 = 1;
                    }
                }
                else{
                    if(rt1[tlist[j][0]] > rt[i]){
                        index2 = 0;
                    }
                    else if(rt1[tlist[j][tlist[j].size()-1]] <= rt[i]){
                        index2 = tlist[j].size();
                    }
                    else{
                        for(int b=0; b<tlist[j].size()-1; b++){
                            if(rt[i]>=rt1[tlist[j][b]] && rt[i]<=rt1[tlist[j][b+1]]){
                                index2 = b+1;
                            }
                        }
                    }
                }
                tlist[j].insert(tlist[j].begin()+index2,i);
                //cout<<index2<<" "<<i<<" "<<j<<endl;
                // initialize ready1 and ready2
                int ready1[10], ready2[10];
                for(int a=0; a<10; a++){
                    ready1[a] = 0;
                }
                for(int a=0; a<10; a++){
                    for(int b=0; b<10; b++){
                        if(g_succ[a][b] == 1){
                            ready1[b] += 1;
                        }
                    }
                    ready2[a] = 1;
                }
                //cout<<ready2[0]<<" "<<i<<" "<<j<<endl;
                for(int a=0; a<4; a++){
                    if(tlist[a].size()>0){
                        ready2[tlist[a][0]] = 0;
                    }
                }

                /*for(int a=0; a<4; a++){
                    for(int b = 0; b<tlist[a].size(); b++){
                        if(b == 0){
                            ready2[tlist[a][b]] = 0;
                        }
                        else{
                            ready2[tlist[a][b]] = 1;
                        }
                    }
                }*/
                //intialize the stack and implement the first operation
                stack<int> s;
                for(int a=0; a<10; a++){
                    if(ready1[a] == 0 && ready2[a] == 0 && pushed[a] == 0){
                        s.push(a);
                        pushed[a] = 1;
                        //cout<<a<<" "<<endl;
                    }
                }
                //cout<<ready2[2]<<" "<<i<<" "<<j<<endl;
                int current1 = s.top();
                s.pop();
                rt[current1] = 0;
                if(core2[current1] == 3){
                    rt[current1] = max(core3[core2[current1]],rt[current1]);
                    ft[current1] = rt[current1] + 5;
                    core3[core2[current1]] = rt[current1] + 3;
                }
                else{
                    rt[current1] = max(core3[core2[current1]],rt[current1]);
                    ft[current1] = rt[current1] + t_l_k[current1][core2[current1]];
                    core3[core2[current1]] = ft[current1];
                }
                //cout<<rt[current1]<<" "<<ft[current1]<<endl;
                //update ready1 and ready2
                for(int a=0; a<10; a++){
                    if(g_succ[current1][a] == 1){
                        ready1[a] -= 1;
                    }
                }
                ready2[current1] = 1;
                //cout<<ready1[5]<<" "<<count<<endl;
                if(tlist[core2[current1]].size()>1){
                    for(int a=1; a<tlist[core2[current1]].size(); a++){
                        if(tlist[core2[current1]][a-1] == current1){
                            ready2[tlist[core2[current1]][a]] = 0;
                            //cout<<tlist[core2[current1]][a]<<" "<<i<<" "<<j<<endl;
                        }
                    }
                }
                //cout<<core2[current1]<<" "<<i<<" "<<j<<endl;
                for(int a=0; a<10; a++){
                    if(ready1[a] == 0 && ready2[a] == 0 && pushed[a] == 0){
                        s.push(a);
                        pushed[a] = 1;
                        //cout<<a<<" ";
                    }
                }
                /*for(int a=0; a<10;a++){
                    cout<<ready2[a]<<" ";
                }*/
                //cout<<rt[0]<<" "<<ft[0]<<" "<<i<<" "<<j<<endl;
                //cout<<ready2[1]<<ready2[2]<<ready2[3]<<ready2[5]<<" "<<i<<" "<<j<<endl;
                //cout<<ready2[1]<<" "<<i<<" "<<j<<endl;
                //cout<<i<<" "<<j<<endl;
                while(s.size() != 0){
                    int current = s.top();
                    s.pop();
                    //cout<<current<<" ";
                    //caculate ready time of current task
                    if(core2[current] == 3){
                        int max_j_ws1 = 0;
                        for(int a=0; a<n; a++){
                            if(g_succ[a][current] == 1 && max_j_ws1 < ft[a]){
                                max_j_ws1 = ft[a];
                            }
                        }
                        rt[current] = max_j_ws1;
                    }
                    else{
                        int max_j_l1 = 0;
                        for(int a=0; a<n; a++){
                            if(g_succ[a][current] == 1 && max_j_l1 < ft[a]){
                                max_j_l1 = ft[a];
                            }
                        }
                        rt[current] = max_j_l1;
                    }
                    if(core2[current] == 3){
                        rt[current] = max(core3[core2[current]],rt[current]);
                        ft[current] = rt[current] + 5;
                        core3[core2[current]] = rt[current] + 3;
                    }
                    else{
                        rt[current] = max(core3[core2[current]],rt[current]);
                        ft[current] = rt[current] + t_l_k[current][core2[current]];
                        core3[core2[current]] = ft[current];
                    }
                    //update ready1 and ready2
                    for(int a=0; a<10; a++){
                        if(g_succ[current][a] == 1){
                            ready1[a] -= 1;
                        }
                    }
                    ready2[current] = 1;
                    if(tlist[core2[current]].size()>1){
                        for(int a=1; a<tlist[core2[current]].size(); a++){
                            if(tlist[core2[current]][a-1] == current){
                                ready2[tlist[core2[current]][a]] = 0;
                            }
                        }
                    }
                    for(int a=0; a<10; a++){
                        if(ready1[a] == 0 && ready2[a] == 0 && pushed[a] == 0){
                            s.push(a);
                            pushed[a] = 1;
                            //cout<<a<<" ";
                        }
                    }
                    //cout<<"s";
                }
                //cout<<endl;
                int current_t = ft[n-1];
                int current_e = 0;
                for(int a=0; a<10; a++){
                    if(core2[a] == 3){
                        current_e += E_c;
                    }
                    else{
                        current_e += E_l[a][core2[a]];
                    }
                }
                //cout<<current_t<<" "<<i<<" "<<j<<endl;
                if(current_t <= t_total && current_e < new_e){
                    less_t1 = 1;
                    new_n = i;
                    new_k = j;
                    new_index1 = index1;
                    new_index2 = index2;
                    new_t = current_t;
                    new_e = current_e;
                    //cout<<new_t<<endl;
                    for(int a=0; a<10; a++){
                        temp_core[a] = core2[a];
                        new_st[a] = rt[a];
                        new_ft[a] = ft[a];
                     }
                }
                if(current_t > t_total && current_t <= tmax && less_t1 == 0 && current_e < e_total && max_ratio < double((e_total - current_e) / (current_t - t_total))){
                    max_ratio = double((e_total - current_e) / (current_t - t_total));
                    //cout<<max_ratio<<endl;
                    less_t2 = 1;
                    new_n = i;
                    new_k = j;
                    new_index1 = index1;
                    new_index2 = index2;
                    new_t = current_t;
                    new_e = current_e;
                    //cout<<new_t<<endl;
                    for(int a=0; a<10; a++){
                        temp_core[a] = core2[a];
                        new_st[a] = rt[a];
                        new_ft[a] = ft[a];
                    }
                }
                //cout<<less_t1<<less_t2<<endl;
                //cout<<new_n<<" "<<new_k<<" "<<i<<" "<<j<<endl;
                //count += 1;
                //cout<<count<<endl;
            }
        }
        if(less_t1 != 1 && less_t2 != 1){
            out = 1;
        }
        else{
            clist[core1[new_n]].erase(clist[core1[new_n]].begin()+new_index1);
            clist[new_k].insert(clist[new_k].begin()+new_index2,new_n);
            t_total = new_t;
            e_total = new_e;
            for(int a=0; a<10; a++){
                core1[a] = temp_core[a];
                st[a] = new_st[a];
                et[a] = new_ft[a];
            }
            if(less_t1 != 1 && less_t2 != 1){
                out = 1;
            }
            count += 1;
            cout<<count<<"th step "<<endl;
            cout<<"Current Operation: Insert Task "<<new_n+1<<" to Core "<<new_k+1<<endl;
            cout<<"Current Completion Time: "<<t_total<< "   Current Energy Consumption: "<<e_total<<endl;
        }
    }
    /*for(int i=0; i<10; i++){
        cout<<core1[i]<<endl;
    }*/
    cout<<endl;
    cout<<"Best Asssignment"<<endl;
    cout<<"Best Energy Consumption: "<<e_total<<"   Best Completion Time: "<<t_total<<endl;
    for(int i=0; i<clist.size(); i++){
        if(i == 3){
            cout<<"Clould: ";
        }
        else{
            cout<<"Core"<<i<<": ";
        }
        for(int j=0; j<clist[i].size(); j++){
            cout<<clist[i][j]+1<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
    cout<<"Time Assignment Details"<<endl;
    /*for(int i=0; i<10; i++){
        cout<<i+1<<"th task from "<<"start "<<st[i]<<" end "<<et[i]<<" at core "<<core1[i]<<endl;
    }*/
    for(int i=0; i<clist.size(); i++){
        if(i == 3){
            cout<<"Cloud: ";
        }
        else{
            cout<<"Core "<<i+1<<": ";
        }
        for(int j=0; j<clist[i].size(); j++){
            cout<<st[clist[i][j]]<<"-Task"<<clist[i][j]+1<<"-"<<et[clist[i][j]]<<" ";
        }
        cout<<endl;
    }
}


int main(int argc, char *argv[])
{
    int t_l_k[][3]={{9,7,5},
                     {8,6,5},
                     {6,5,4},
                     {7,5,3},
                     {5,4,2},
                     {7,6,4},
                     {8,5,3},
                     {6,4,2},
                     {5,3,2},
                     {7,4,2}};
    /*int t_l_k[][3]={{5,7,9},
                     {5,6,8},
                     {4,5,6},
                     {3,5,7},
                     {2,4,5},
                     {4,6,7},
                     {2,5,8},
                     {2,4,6},
                     {2,3,5},
                     {2,4,7}};*/
    //cout<<t_l_k[2][9];
    int g_succ[][10]={{0,1,1,1,1,1,0,0,0,0},
                    {0,0,0,0,0,0,0,1,1,0},
                    {0,0,0,0,0,0,1,0,0,0},
                    {0,0,0,0,0,0,0,1,1,0},
                    {0,0,0,0,0,0,0,0,1,0},
                    {0,0,0,0,0,0,0,1,0,0},
                    {0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,0}}; //adjancy matrix
    /*int g_succ[][10]={{0,1,1,1,1,1,0,0,0,0},
                    {0,0,0,0,0,0,1,0,0,0},
                    {0,0,0,0,0,0,1,0,0,0},
                    {0,0,0,0,0,0,0,1,0,0},
                    {0,0,0,0,0,0,0,0,1,0},
                    {0,0,0,0,0,0,0,0,1,0},
                    {0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,1},
                    {0,0,0,0,0,0,0,0,0,0}};*/
    int C[10]; //local:0, cloud:1
    int pri[10], pri_n[10], w[10], core[4], core1[10];
    int rt_l[10], rt_c[10], rt_ws[10], ft_ws[10], ft_wr[10], ft_l[10], ft[10];
    vector<vector<int>> clist(4);
    //list<int> c1, c2, c3, c0;
    //int clist[4][10]
    for(int i=0; i<10; i++){
        C[i] = 0;
        pri[i] = 0;
        pri_n[i] = 0;
        w[i] = 0;
        rt_l[i] = 0;
        rt_ws[i] = 0;
        rt_c[i] = 0;
        ft_ws[i] = 0;
        ft_wr[i] = 0;
        ft_l[i] = 0;
        ft[i] = 0;
        core1[i] = 0;
    }
    for(int i=0; i<4; i++){
        core[i] = 0;
    }
    int k = 3, n = 10;
    int t_s = 3, t_c = 1, t_r = 1;
    int t_re = t_s + t_c + t_r;
    int E_l[10][3];
    int pk[] = {1,2,4};
    float ps = 0.5;
    float E_c = ps * t_s;
    for(int i=0; i<10; i++){
      for(int j=0; j<3; j++){
          E_l[i][j] = pk[j] * t_l_k[i][j];
      }
    }
    auto start = chrono::high_resolution_clock::now();
    ios_base::sync_with_stdio(false);
    primary_assignment(t_l_k,C,t_re,n,k);
    task_prioritzing(t_l_k,pri,pri_n,g_succ,w,C,t_re,n,k);
    /*for(int i=0; i<n; i++){
        cout<<pri_n[i]<<endl;
    }*/
    execution_unit_selection(t_l_k,pri_n,g_succ,C,rt_l,rt_c,rt_ws,ft_ws,ft_wr,ft_l,ft,core,core1,n,k,t_s,t_r,t_c,clist);
    /*for(int i=0; i<10; i++){
        int rt = max(rt_ws[i],rt_l[i]);
        cout<<rt<<" - "<<ft[i]<<endl;
    }*/
    for(int i=0; i<4; i++){
        for(int j=0; j<10; j++){
            if(core1[j] == i){
                clist[i].push_back(j);
            }
        }
    }
    float e_total = 0;
    for(int i=0; i<10; i++){
        if(core1[i] == 3){
            e_total += E_c;
        }
        else{
            e_total += E_l[i][core1[i]];
        }
    }
    int st[10];
    for(int i=0; i<10; i++){
        st[i] = max(rt_l[i],rt_ws[i]);
        //cout<<st[i]<<endl;
        //cout<<core1[i]<<" ";
    }
    /*for(int i=0; i<clist.size(); i++){
        for(int j=0; j<clist[i].size(); j++){
            cout<<clist[i][j]<<" ";
        }
        cout<<endl;
    }*/
    //cout<<e_total;
    int tmin = ft[n-1];
    int tmax = 27;
    cout<<"Initial Scheduling (Format: Start Time - Task Number - Finish Time)"<<endl;
    for(int i=0; i<clist.size(); i++){
        if(i == 3){
            cout<<"Cloud: ";
        }
        else{
            cout<<"Core"<<i+1<<": ";
        }
        for(int j=0; j<clist[i].size(); j++){
            cout<<st[clist[i][j]]<<"-Task"<<clist[i][j]+1<<"-"<<ft[clist[i][j]]<<" ";
        }
        cout<<endl;
    }
    cout<<"Initial Energy Consumption: "<<e_total<<"   Initial Completion Time: "<<tmin<<endl;
    cout<<endl;
    kernel(clist,t_l_k,g_succ,core1,tmax,tmin,e_total,10,3,st,ft,E_c,E_l);
    /*for(int i=0; i<10; i++){
        cout<<core1[i]<<" ";
    }*/
    auto end = chrono::high_resolution_clock::now();
    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;
    cout << endl << "Time taken by program is : " << fixed << time_taken << setprecision(9);
    cout << " sec" << endl;
    return 0;
}
