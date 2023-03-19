Timer::Timer(long interval,auto& lambda,bool repeats):last_update(std::clock()),repeats(repeats),
interval(interval){
    function=lambda;
}