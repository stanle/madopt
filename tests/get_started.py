import madopt

def getModel():
    N = 10**5
    model = madopt.IpoptModel(show_solver=True)
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

    return model

model = getModel()
model.solve()
