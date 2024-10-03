OPTYPE_REGISTER = 0
OPTYPE_IMMEDIATE = 1
OPTYPE_CONDITIONAL = 2

REGS_LIST = {f"X{i+1}": i for i in range(16)}.extend({"XRV": 13, "XFP": 14, "XSP": 15})
COND_LIST = {"EQ": 0, "LT": 1, "GT": 2, "LE": 3, "GE": 4, "NE": 5, "ZR": 0, "NZ": 1}
