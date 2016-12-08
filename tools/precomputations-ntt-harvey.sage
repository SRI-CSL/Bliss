#
# Script to produce the precomputed values for the implementations
# (mostly for the NTT)
# 

# Parameters
n = 1024
q = 12289

# Bit reverse for nbits in [1, 16]
def reverse(b, nbits=8):
    def reverse8(b):
        b = (b & 0xF0) >> 4 | (b & 0x0F) << 4
        b = (b & 0xCC) >> 2 | (b & 0x33) << 2
        b = (b & 0xAA) >> 1 | (b & 0x55) << 1
        return b
    if nbits<8:
        return reverse8(b)>>(8-nbits)
    if nbits==8:
        return reverse8(b)
    if nbits>8:
        lsb = b & ( (1<<(nbits-8))-1 )
        msb = b >> (nbits-8)
        return reverse8(msb)|reverse(lsb, nbits=nbits-8)<<8

# Table of bitreverse
def bitreverse(N):
    return [reverse(b, (N-1).nbits()) for b in range(N)]

# Define the rings
Zq.<X> = Integers(q)[]
Rq.<x> = PolynomialQuotientRing(Zq, X ^ n+1)

# roots of x^n+1 (psi) and its square (omega: root of x^2n+1)
f = X ^ n+1
psi = f.roots()[-1][0]
omega = psi ^ 2

# Prepare tables
def prep_wtab(om, inverse = False):
    if inverse:
        w = (om^(-1)).lift()
    else:
        w = om.lift()
    oms = []
    oms_shoup = []
    K = n
    while (K >= 2):
        wi = 1 # w^i
        for i in range(K/2):
            oms += [wi]
            oms_shoup += [floor((wi<<16)/q)]
            wi = (wi*w)%q
        w = (w*w)%q
        K /= 2
    wi = 1
    return (oms, oms_shoup)

def prep_wtab_psi(ps, inverse = False):
    if inverse:
        pss = [(ps^(-i)*(n^(-1)%q)).lift() for i in range(n)]
    else:
        pss = [(ps^i).lift() for i in range(n)]
    pss_shoup = [floor((pss[i]<<16)/q) for i in range(n)]
    return (pss, pss_shoup)

# powers of omega
omegas, omegas_shoup = prep_wtab(omega)
omegas_inv, omegas_inv_shoup = prep_wtab(omega, inverse=True)
psis, psis_shoup = prep_wtab_psi(psi)
psis_inv, psis_inv_shoup = prep_wtab_psi(psi, inverse =True)

R = bitreverse(n)
omegas_inv_bitreverse = [omegas_inv[R[i]] for i in range(len(omegas_inv))]
omegas_inv_shoup_bitreverse = [omegas_inv_shoup[R[i]] for i in range(len(omegas_inv_shoup))]

print "uint16_t omegas[%d] = {\n"%len(omegas),
for i in range((len(omegas)+1)/16):
	print "\t\t",
	for j in range(16):
		if i*16+j < len(omegas)-1: print "%d,"%omegas[16*i+j],
		elif i*16+j == len(omegas)-1: print "%d};\n"%omegas[len(omegas)-1]
	print "\n",
print "uint16_t omegas_shoup[%d] = {\n"%len(omegas_shoup),
for i in range((len(omegas_shoup)+1)/16):
	print "\t\t",
	for j in range(16):
		if i*16+j < len(omegas_shoup)-1: print "%d,"% omegas_shoup[16*i+j],
		elif i*16+j == len(omegas_shoup)-1: print "%d};\n"% omegas_shoup[len(omegas_shoup)-1]
	print "\n",
print "uint16_t omegas_inv[%d] = {\n"%len(omegas_inv),
for i in range((len(omegas_inv)+1)/16):
	print "\t\t",
	for j in range(16):
		if i*16+j < len(omegas_inv)-1: print "%d,"% omegas_inv[16*i+j],
		elif i*16+j == len(omegas_inv)-1: print "%d};\n"% omegas_inv[len(omegas_inv)-1]
	print "\n",
print "uint16_t omegas_inv_shoup[%d] = {\n"%len(omegas_inv_shoup),
for i in range((len(omegas_inv_shoup)+1)/16):
	print "\t\t",
	for j in range(16):
		if i*16+j < len(omegas_inv_shoup)-1: print "%d,"% omegas_inv_shoup[16*i+j],
		elif i*16+j == len(omegas_inv_shoup)-1: print "%d};\n"% omegas_inv_shoup[len(omegas_inv_shoup)-1]
	print "\n",
print "uint16_t psis[%d] = {\n"%len(psis),
for i in range((len(psis)+1)/16):
	print "\t\t",
	for j in range(16):
		if i*16+j < len(psis)-1: print "%d,"%psis[16*i+j],
		elif i*16+j == len(psis)-1: print "%d};\n"% psis[len(psis)-1]
	print "\n",
print "uint16_t psis_shoup[%d] = {\n"%len(psis_shoup),
for i in range((len(psis_shoup)+1)/16):
	print "\t\t",
	for j in range(16):
		if i*16+j < len(psis_shoup)-1: print "%d,"% psis_shoup[16*i+j],
		elif i*16+j == len(psis_shoup)-1: print "%d};\n"% psis_shoup[len(psis)-1]
	print "\n",
print "uint16_t psis_inv[%d] = {\n"%len(psis_inv),
for i in range((len(psis_inv)+1)/16):
	print "\t\t",
	for j in range(16):
		if i*16+j < len(psis_inv)-1: print "%d,"% psis_inv[16*i+j],
		elif i*16+j == len(psis_inv)-1: print "%d};\n"% psis_inv[len(psis_inv)-1]
	print "\n",
print "uint16_t psis_inv_shoup[%d] = {\n"%len(psis_inv_shoup),
for i in range((len(psis_inv_shoup)+1)/16):
	print "\t\t",
	for j in range(16):
		if i*16+j < len(psis_inv_shoup)-1: print "%d,"% psis_inv_shoup[16*i+j],
		elif i*16+j == len(psis_inv_shoup)-1: print "%d};\n"% psis_inv_shoup[len(psis_inv)-1]
        print "\n",