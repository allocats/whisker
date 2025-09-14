#ifndef MACROS_H
#define MACROS_H

#define UNLIKELY(x) __builtin_expect(x, 0)
#define LIKELY(x) __builtin_expect(x, 1)

#define W_TODO(msg) fprintf(stdout, "%s:%d: \e[1mTODO\e[0m: %s\n", __FILE__, __LINE__, msg)

#endif // !MACROS_H
