def MinJaccard ( a, b ):
    matches = 0.0
    for m in a:
        if m in b:
            matches = matches + 1

    return float ( matches ) / min ( len ( a ), len ( b ) )
