#include "../tests/testmodel.hpp"
#include "../src/common.hpp"
#include "../src/inner_constraint.hpp"

using namespace std;

int main(){
    TestModel m;
    Var a = m.addVar("a");
    Var b = m.addVar("b");
    auto exp = cos(a * b);

    vector<double> x = {1, 1};

    HessPosMap hess_pos_map;
    auto& simstack = m.getSimStack();
    simstack.setXSize(x.size());
    InnerConstraint constraint(exp, 0, 0, hess_pos_map, simstack);

    auto& cstack = m.getCStack();
    cstack.resize(simstack);
    cstack.setX(x.data());
    constraint.setEvals(cstack);

    for (auto& p: hess_pos_map) {
	    cout << p.first << " " << p.second << endl;
    }
    
    for (auto& p: constraint.getHessMap()) {
	    cout << p << endl;
    }
}
