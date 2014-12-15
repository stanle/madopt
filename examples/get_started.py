import madopt

N = 10**5

model = madopt.IpoptModel(show_solver=True)

model.timelimit = 1000

#Binary Variable
#var = model.addBVar(init=0, name="var")

#Integer 
#var = model.addIVar(lb=-12, ub=33, init=0, name="var")

#Continous
#var = model.addVar(lb=-12, ub=33, init=0, name="var")

x = dict()
for i in range(N):
    x[i] = model.addVar(lb=-1.5, ub=0, init=-0.5, name="x"+str(i))

obj = madopt.Expr(0)
for i in range(N):
    obj += (x[N-i-1] - 1)**2
model.setObj(obj)

for i in range(N-2):
    a = float(i+2)/float(N)
    model.addConstr((x[i+1]**2 + 1.5*x[i+1] - a)*madopt.cos(x[i+2]) - x[i], lb=0, ub=0)

model.solve()

if model.has_solution:
    print(model.nx, model.ng, model.objValue, model.stat)

var = x[0]
if model.has_solution:
    # var.x ==> solution value
    print(var.lb, var.ub, var.init, var.x)

# change bounds
var.lb = -1
var.ub = 12

#set Solution as next init values
#m.solAsInit()
