import numpy as np

import matplotlib.pyplot as plt


# Continuous signal parameters

fs = 25   # sampling frequency (Hz) - lower to show fewer samples

f_signal = 2  # signal frequency (Hz)

t_cont = np.linspace(0, 1, 1000)  # continuous time

x_cont = np.sin(2 * np.pi * f_signal * t_cont)  # continuous signal


# Discrete signal (sampling)

Ts = 1/fs

n = np.arange(0, 1, Ts)

x_disc = np.sin(2 * np.pi * f_signal * n)


plt.figure(figsize=(10,5))

plt.plot(t_cont, x_cont, 'b-', label="Αναλογικό σήμα x(t)")

plt.stem(n, x_disc, linefmt='r-', markerfmt='ro', basefmt=" ", label="Δείγματα x[n]")

plt.plot(n, x_disc, 'r--', linewidth=1, alpha=1, label="Ψηφιακό Σήμα")  # γραμμή που ενώνει τα δείγματα

plt.xlabel("Χρόνος (s)")

plt.ylabel("Πλάτος")

plt.title("Μετατροπή Αναλογικού Σήματος σε Ψηφιακό Σήμα")

plt.legend()

plt.grid(True)

plt.tight_layout()

plt.show()
