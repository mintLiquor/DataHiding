//
//  main.cpp
//  datahiding
//
//  Created by tianran on 2020/6/15.
//  Copyright © 2020 TianRan. All rights reserved.
//

#include"STDM.hpp"
#include<iomanip>
#define MAX 100000
using namespace std;

int main() {
    STDM s;

    double X[NUM];
    double S[NUM];
    double V[NUM];
    double Y[NUM];
    
    /*
    double std=0.5;
    double delta=2.0;
    double cnt=0.0;
    double theo=0.0;
    for (int i=0;i<MAX;i++){
        s.GaussGen(X, 10);
        //for (int k=0;k<NUM;k++)
          //  cout<<X[k]<<" ";
        s.inset(X, delta, S);
        s.addGaussAttack(std, S, V, Y);
        if(s.decode(Y, delta)!=1)
            cnt+=1.0;
        
        theo+=s.CalculateTheo(V, delta,std);
    }
     cout << "当步长为4，STD为2时，实验错误率为：" << cnt / MAX << "；理论错误率为：" << theo/MAX << endl;
     */
    
    
    //步长delta=4，std为0.5、1、1.5、2、2.5、3、3.5、4、4.5、5
    
    double delta = 4.0;
    double cnt = 0.0;
    for (double i = 0.5; i < 5.5; i += 0.5) {
        for (int n = 0; n < MAX; n++) {
            s.GaussGen(X, 10);
            s.inset(X, delta, S);
            s.addGaussAttack(i, S, V, Y);
            s.decode(Y, delta);//解码后得出de
            if (s.decode(Y, delta) != 1.0)
                cnt += 1.0;
        }
        //cout<<"Q(0)="<<s.Q(0)<<"Q(1)="<<s.Q(1)<<endl;
        cout << "当步长为4，STD为" << i << "时，实验错误率为：" << cnt / MAX << "；理论错误率为：" << s.CalculateTheo(V, delta,i) << endl;
        cnt=0.0;
    }

    //当STD=2，步长为0.2，0.4，0.6⋯⋯，2.0
    double std[2] = { 2, 4 };
    cnt = 0.0;
    for (int i = 0; i < 2; i++) {
        for (double delta = 0.2; delta <= 2.0; delta += 0.2) {
            for (int t = 0; t < MAX; t++) {
                s.GaussGen(X, 10);
                s.inset(X, delta, S);
                s.addGaussAttack(std[i], S, V, Y);
                s.decode(Y, delta);//解码后得出de
                if (s.decode(Y, delta) != 1.0)
                    cnt += 1.0;
            }
            //cout<<"Q(0)="<<s.Q(0)<<endl;
            cout << "当std为" << std[i] << "，嵌入步长为 " << delta << "时，实验错误率为：" << cnt / MAX << "；理论错误率为：" << s.CalculateTheo(V, delta,std[i]) << endl;
            cnt=0.0;
        }
    }
    
    return 0;
}
