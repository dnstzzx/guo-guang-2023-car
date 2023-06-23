#pragma once

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

#define limit(in, minn, maxx)   (min(max(in, minn), maxx))

#define swap(a, b) do { typeof(a) temp = a; a = b; b = temp;} while(0)
