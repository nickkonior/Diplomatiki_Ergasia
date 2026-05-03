import numpy as np

import matplotlib.pyplot as plt


# Common discrete-time axis

N = 200

n = np.arange(0, N)


# 1) Pure delay: h[n] = δ[n - D]

D_delay = 40

h_delay = np.zeros_like(n, dtype=float)

h_delay[D_delay] = 1.0


plt.figure(figsize=(9, 3.5))

(markerline, stemlines, baseline) = plt.stem(n, h_delay, basefmt=" ")

plt.title("Κρουστική απόκριση καθαρού delay: h[n] = δ[n−D]")

plt.xlabel("n")

plt.ylabel("h[n]")

plt.grid(True, alpha=0.3)

plt.tight_layout()

plt.savefig("/mnt/data/impulse_response_delay.png", dpi=200)

plt.show()


# 2) Echo: h[n] = δ[n] + α δ[n − D]

D_echo = 50

alpha = 0.6

h_echo = np.zeros_like(n, dtype=float)

h_echo[0] = 1.0

if D_echo < N:

    h_echo[D_echo] = alpha


plt.figure(figsize=(9, 3.5))

plt.stem(n, h_echo, basefmt=" ")

plt.title("Κρουστική απόκριση echo: h[n] = δ[n] + α δ[n−D]")

plt.xlabel("n")

plt.ylabel("h[n]")

plt.grid(True, alpha=0.3)

plt.tight_layout()

plt.savefig("/mnt/data/impulse_response_echo.png", dpi=200)

plt.show()


# 3) Reverb-like IR: sparse, exponentially decaying reflections

np.random.seed(7)

h_reverb = np.zeros_like(n, dtype=float)

num_reflections = 40

times = np.sort(np.random.choice(np.arange(5, N), size=num_reflections, replace=False))

decay = 0.98  # exponential decay per sample

for t in times:

    amp = (decay ** t) * (0.5 + 0.5*np.random.rand())  # decaying and somewhat random amplitude

    h_reverb[t] = amp


plt.figure(figsize=(9, 3.5))

plt.stem(n, h_reverb, basefmt=" ")

plt.title("Κρουστική απόκριση τύπου reverb (πολλαπλές αποσβεννυόμενες ανακλάσεις)")

plt.xlabel("n")

plt.ylabel("h[n]")

plt.grid(True, alpha=0.3)

plt.tight_layout()

plt.savefig("/mnt/data/impulse_response_reverb.png", dpi=200)

plt.show()


print("Αποθηκευμένα αρχεία:")

print("- /mnt/data/impulse_response_delay.png")

print("- /mnt/data/impulse_response_echo.png")

print("- /mnt/data/impulse_response_reverb.png")

