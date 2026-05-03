clc;

clear;

close all;


%% Παράμετροι σήματος

fs = 44100;              % Συχνότητα δειγματοληψίας (Hz)

t = 0:1/fs:0.01;         % Χρονικός άξονας (10 ms)

f = 1000;                % Συχνότητα ημιτόνου (Hz)


x = sin(2*pi*f*t);       % Σήμα εισόδου


%% Παράμετροι delay

D = 200;                 % Καθυστέρηση σε δείγματα

alpha = 0.7;             % Συντελεστής εξασθένησης


%% Δημιουργία καθυστερημένου σήματος

x_delayed = [zeros(1, D), x(1:end-D)];


%% Έξοδος (input + delayed)

y = x + alpha * x_delayed;


%% Οπτικοποίηση

figure;


subplot(3,1,1);

plot(t, x);

title('Σήμα Εισόδου x[n]');

xlabel('Χρόνος (s)');

ylabel('Πλάτος');


subplot(3,1,2);

plot(t, x_delayed);

title('Καθυστερημένο Σήμα x[n-D]');

xlabel('Χρόνος (s)');

ylabel('Πλάτος');


subplot(3,1,3);

plot(t, y);

title('Έξοδος y[n] = x[n] + \alpha x[n-D]');

xlabel('Χρόνος (s)');

ylabel('Πλάτος');

