对于该并发编程问题，我采取了两阶段锁协议的方式，避免发生读写锁死锁问题。


(a) 考虑并发保护，我将读操作和写操作都设计为原子操作。我使用了临时变量来存储数组值，并在计算完毕后再进行写操作。\
S_i0 = S[i];\
S_i1 = S[(i+1)%N];\
S_i2 = S[(i+2)%N];\
S[j] = S_j;\

(b) 锁的粒度被设计得尽量小，shared_mutex数组被设计成和数组等长，即每个元素分配一个锁。\

(c) 读锁一般不会影响并发度，因为多个读锁可并行对同一个资源进行锁操作。但是写锁要求不能有读锁或其他写锁上锁该资源，所以限制了并发度。\

(d) 考虑到j可能在[i,i+2]范围内，这也是常见的导致死锁的原因。我使用了临时变量来存储读取的结果，并在所有读锁释放之后，再获取写锁进行写操作。\
    如此，读写操作就是分离的，不会导致持有读锁的进程，在尝试读取被写锁锁定的元素的时候，进入hold and wait状态并导致死锁。\
    
(e) 在我初始的设计尝试中，如果不对读写操作进行分离，死锁常会发生，这是因为线程1的写锁可能阻塞线程2的读锁，而线程2的读锁会阻塞线程1的写锁。\
    采取两阶段锁协议之后（扩展-收缩），同时分离了读写操作，可以保证一个线程在一个时刻仅处于读状态或者写状态，避免了死锁。
