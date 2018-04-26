function [] = jwrite( depth, lambda, movingWindowSize, lookBuffer, lookSide, minPeakDistance, minPeakHeight, startPoint, markovDependencySwitch, normalizeZerothOrders, numberOfDependenciesConsidered, discountFactor)
    
	p.parameters.depth = str2num(depth);
    p.parameters.lambda = str2num(lambda);
    p.parameters.movingWindowSize = str2num(movingWindowSize);
    p.parameters.lookBuffer = str2num(lookBuffer);
    p.parameters.lookSide = str2num(lookSide);
    p.parameters.minPeakDistance = str2num(minPeakDistance);
    p.parameters.minPeakHeight = str2num(minPeakHeight);
    p.parameters.startPoint = str2num(startPoint);
    p.parameters.markovDependencySwitch = markovDependencySwitch;
    p.parameters.normalizeZerothOrders = str2num(normalizeZerothOrders);
    p.parameters.numberOfDependenciesConsidered = str2num(numberOfDependenciesConsidered);
    p.parameters.discountFactor = str2num(discountFactor);
    jsonwrite('../parameters.json', p);

end