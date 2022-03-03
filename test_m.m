[x,fs] = audioread("test_file.wav");

y=vibrato(x,44100,10,0.05);

[N,~]=size(y);

figure(1);clf


subplot(111);
time=1:1:N;
plot(time,y)
ylabel('Amplitude')
xlabel('Samples')
title('samplerat=44100,modfreq=10,width=0.05,matlab');