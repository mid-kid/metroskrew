int func2();

int func()
{
    int var = 0;
    while (var) var--;

    func2(0);
    if (var) {
        if (0) {
            int var = 0;
            func2(0);
            return 0;
        }
        return 0;
    }
    return 0;
}
