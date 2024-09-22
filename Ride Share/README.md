# Ride Share
A simulated ride-share app with requests concurently being generated and fulfilled using pthreads and semaphores to enforce mutual exlusion.
Requests are communicated between producers and consumers through a bounded buffer struct shared between all active threads.
