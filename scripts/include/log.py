def print_error(msg: str):
    print(f"\033[31mERROR: {msg}\033[0m")


def print_warning(msg: str):
    print(f"\033[33mWarning: {msg}\033[0m")


def print_warnf(msg: str):
    print(f"\033[33m{msg}\033[0m")


def print_ok(msg: str):
    print(f"\033[32m{msg}\033[0m")