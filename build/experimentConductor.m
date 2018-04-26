function [] = experimentConductor ()

	% parameters
    depthList = {'20', '30', '50', '100'};
    lambdaList = {'0.85', '0.9', '0.95', '0.99'};
    movingWindowSizeList = {'30', '50', '100'};
    lookBufferList = {'20'};
    lookSideList = {'1'};
    minPeakDistanceList = {'500'};
    minPeakHeightList = {'0.2', '0.25', '0.3', '0.35', '0.4'};
    startPointList = {'500'};
	markovDependencySwitchList = {'001'};                                                       % zfs
	normalizeZerothOrdersList = {'1'};																							% 0: no, 1:yes
	numberOfDependenciesConsideredList = {'16', '0'};										% 0: auto
	discountFactorList = {'0.50', '0.90', '0.99', '0'};														% 0: linear
	
	excelFileName = 'experimentLogs.xlsx';
	if exist(excelFileName, 'file') == 2
		delete(excelFileName);
	end
	
	logTypes = {'F1 Score', 'Precision', 'Recall', 'depth', 'lambda', 'movingWindowSize', 'lookBuffer', 'lookSide', 'minPeakDistance', 'minPeakHeight', 'startPoint', 'markovDependencySwitch', 'normalizeZerothOrders', 'numberOfDependenciesConsidered', 'discountFactor'};
	block = ['A1:' char(64+size(logTypes,2)) '1'];
	xlswrite(excelFileName, logTypes, block);
	
	currentExp = 1;
    for depthNum = 1:numel(depthList)

        depth = depthList{depthNum};

        for lambdaNum = 1:numel(lambdaList)

            lambda = lambdaList{lambdaNum};

            for movingWindowSizeNum = 1:numel(movingWindowSizeList)

                movingWindowSize = movingWindowSizeList{movingWindowSizeNum};

                for lookBufferNum = 1:numel(lookBufferList)

                    lookBuffer = lookBufferList{lookBufferNum};
                    
                    for lookSideNum = 1:numel(lookSideList)

                        lookSide = lookSideList{lookSideNum};

                        for minPeakDistanceNum = 1:numel(minPeakDistanceList)

                            minPeakDistance = minPeakDistanceList{minPeakDistanceNum};

                            for minPeakHeightNum = 1:numel(minPeakHeightList)

                                minPeakHeight = minPeakHeightList{minPeakHeightNum};

                                for startPointNum = 1:numel(startPointList)

                                    startPoint = startPointList{startPointNum};
                                    
                                    for markovDependencySwitchNum = 1:numel(markovDependencySwitchList)
	
                                        markovDependencySwitch = markovDependencySwitchList{markovDependencySwitchNum};

                                        for normalizeZerothOrdersNum = 1:numel(normalizeZerothOrdersList)

                                            normalizeZerothOrders = normalizeZerothOrdersList{normalizeZerothOrdersNum};

                                            for numberOfDependenciesConsideredNum = 1:numel(numberOfDependenciesConsideredList)

                                                numberOfDependenciesConsidered = numberOfDependenciesConsideredList{numberOfDependenciesConsideredNum};

                                                for discountFactorNum = 1:numel(discountFactorList)

                                                    discountFactor = discountFactorList{discountFactorNum};
													
													jwrite(depth, lambda, movingWindowSize, depth, lookSide, minPeakDistance, minPeakHeight, startPoint, markovDependencySwitch, normalizeZerothOrders, numberOfDependenciesConsidered, discountFactor);
													
                                                    % experiment
                                                    expName = ['md' markovDependencySwitch '_norm' normalizeZerothOrders '_ln' numberOfDependenciesConsidered '_df' discountFactor];
                                                    % dos(['runBulk.bat ' markovDependencySwitch ' ' normalizeZerothOrders ' ' numberOfDependenciesConsidered ' ' discountFactor]);
                                                    system('start /MIN /wait runBulk.bat');
                                                    
													fpr = resultAnalyser(100, 'absolute', 50);
                                                    fprMatrix = mean(fpr);
                                                    avgF1 = fprMatrix(1);
                                                    avgP = fprMatrix(2);
                                                    avgR = fprMatrix(3);
                                                    % dlmwrite(['../fprLogs/' expName], fpr);
                                                    % copyfile('../result3.txt', ['../fprLogs/' expName]);

                                                    % xlswrite(excelFileName, expName, ['A' currentExp+1]);
                                                    % values = [avgF1 avgP avgR {['m' markovDependencySwitch]} normalizeZerothOrders numberOfDependenciesConsidered discountFactor];
                                                    values = [avgF1 avgP avgR depth lambda movingWindowSize depth lookSide minPeakDistance minPeakHeight startPoint {['m' markovDependencySwitch]} normalizeZerothOrders numberOfDependenciesConsidered discountFactor];
                                                    block = ['A' num2str(currentExp+1) ':' char(64+size(logTypes,2)) num2str(currentExp+1)];
                                                    xlswrite(excelFileName, values, block);
                                                    currentExp = currentExp + 1
                                                end
												
                                            end
											
                                        end
										
                                    end

                                end

                            end

                        end

                    end
					
                end
				
            end
			
        end
		
    end
end