#
# Script to generate KAT values for poly
#

REPETITIONS = 100

n = 512
q = 12289
Zq.<X> = Integers(q)[]
Rq.<x> = PolynomialQuotientRing(Zq, X ^ n+1)

f = open('../tests/static/data_poly_blzzd.c', 'w')

f.write('#define REPETITIONS %d\n\n'%REPETITIONS)

f.write('#define N %d\n\n'%n)

f.write('int32_t a[REPETITIONS][N], b[REPETITIONS][N], c[REPETITIONS][N];\n')

f.write('\n')

for i in range(REPETITIONS):
    a = Rq.random_element()
    f.write('int32_t a%d[%d]={'%(i,n))
    for j in range(n-1):
        f.write('%d, '%a[j].lift())
    f.write('%d};\n'%a[n-1].lift())
    f.write('for (size_t i=0; i<%d; i++) a[%d][i] = a%d[i];\n'%(n,i,i))
    b = Rq.random_element()
    f.write('int32_t b%d[%d]={'%(i,n))
    for j in range(n-1):
        f.write('%d, '%b[j].lift())
    f.write('%d};\n'%b[n-1].lift())
    f.write('for (size_t i=0; i<%d; i++) b[%d][i] = b%d[i];\n'%(n,i,i))
    c = a*b
    f.write('int32_t c%d[%d]={'%(i,n))
    for j in range(n-1):
        f.write('%d, '%c[j].lift())
    f.write('%d};\n'%c[n-1].lift())
    f.write('for (size_t i=0; i<%d; i++) c[%d][i] = c%d[i];\n'%(n,i,i))
    f.write('\n')

f.close()
