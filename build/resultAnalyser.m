% absolute:	trueRange points on left and trueRange points on right are accepted
% percent:	%trueRange points on left and %trueRange points on right are accepted

function [matrix] = resultAnalyser (numOfPoints, mode, trueRange)
	% mode detection
	if strcmp(mode, 'absolute') == 1
		halfRange = trueRange;
	elseif strcmp(mode, 'percent') == 1
		trueRangePercent = trueRange / 100;
		halfRange = floor(numOfPoints * trueRangePercent);
	else
		disp('Invalid mode!');
		return;
	end
        
    truePositive_total = 0;
    trueNegative_total = 0;
    falsePositive_total = 0;
    falseNegative_total = 0;
    numOfExp = 0;
    
	result1 = fopen('../result1.txt');
    result3 = fopen('../result3.txt', 'w');
	
    result1Line = fgets(result1);
    while (ischar(result1Line))
	
		[result1_expName, result1_changePointsText] = strtok(result1Line, ':');
		result1_expName = strtrim(result1_expName);
		result1_changePointsText = strtrim(result1_changePointsText(2:end));
		result1_changePoints = str2double(regexp(result1_changePointsText, ' ', 'split'));
		acceptedPoints = result1_changePoints;
		numOfChangePoints = size(acceptedPoints, 2);
		
		for i = 1:numOfChangePoints
        
			point = acceptedPoints(i);
			leftHalf = zeros(1, halfRange);
			rightHalf = zeros(1, halfRange);
			
			for j = 1:halfRange
				leftHalf(j) = point - j;
				rightHalf(j) = point + j;
			end
			
			acceptedPoints = [acceptedPoints, leftHalf, rightHalf];
        
		end

        truePositive = 0;
        trueNegative = 0;
        falsePositive = 0;
        falseNegative = 0;
		
		falsePositiveList = [];
		falseNegativeList = [];
		
		result2 = fopen('../result2.txt');
        result2Line = fgets(result2);
		while (ischar(result2Line))
			[expName, foundPointsText] = strtok(result2Line, ':');
			expName = strtrim(expName);
			if (strcmp(expName, result1_expName))
				 fclose(result2);
				break;
			end
			result2Line = fgets(result2);
		end
		
		expName = strtrim(expName);
		foundPoints = str2num(strtrim(foundPointsText(2:end)));
        numOfFoundPoints = size(foundPoints, 2);	
        for i = 1:numOfFoundPoints            
            point = foundPoints(i);            
            if (any(acceptedPoints == point) == 1)
                truePositive = truePositive + 1;
            else
                falsePositive = falsePositive + 1;
				falsePositiveList = [falsePositiveList point];
            end
        end
        
        falseNegative = numOfChangePoints - truePositive;
        trueNegative = numOfPoints - truePositive - falsePositive - falseNegative;
		
		for neg = 1:numOfChangePoints
			point = result1_changePoints(neg);
			if (isempty(find(foundPoints >= point-halfRange & foundPoints <= point+halfRange)))
                falseNegativeList = [falseNegativeList point];
			end
		end

        truePositive_total = truePositive_total + truePositive;
        trueNegative_total = trueNegative_total + trueNegative;
        falsePositive_total = falsePositive_total + falsePositive;
        falseNegative_total = falseNegative_total + falseNegative;
        precision = truePositive / (truePositive + falsePositive);
        recall = truePositive / (truePositive + falseNegative);
		if (precision + recall == 0)
			f1 = 0;
		else
			f1 = (2 * precision * recall) / (precision + recall);
		end
		
        fprintf(result3, '%s:\t#f1: %f\t|\tpre: %f\t|\trec:%f\t|\t#tp: %d\t|\t#tn: %d\t|\t#fp: %d\t|\t#fn: %d', expName, f1, precision, recall, truePositive, trueNegative, falsePositive, falseNegative);
		
		if size(falsePositiveList, 2) ~= 0
			fprintf(result3, ' ----- fps: ');
		end
		for p = 1:size(falsePositiveList, 2)
			fprintf(result3, '%d ', falsePositiveList(p));
		end
		if size(falseNegativeList, 2) ~= 0
			fprintf(result3, ' ----- fns: ');
		end
		for p = 1:size(falseNegativeList, 2)
			fprintf(result3, '%d ', falseNegativeList(p));
		end
		fprintf(result3, '\n');
		
		result1Line = fgets(result1);
        numOfExp = numOfExp + 1;
		
		% constructing the matrix that the function returns
		matrix(numOfExp, 1) = f1;
		matrix(numOfExp, 2) = precision;
		matrix(numOfExp, 3) = recall;
		
    end

    fclose(result1);
    
    totalNumOfChangePoints = numOfChangePoints * numOfExp;
    
    a = truePositive_total;
    b = falsePositive_total;
    c = falseNegative_total;
    d = trueNegative_total;
	precision = a / (a + b);
	recall = a / (a + c);
	if (precision + recall == 0)
		f1 = 0;
	else
		f1 = (2 * precision * recall) / (precision + recall);
	end
    sensitivity = a / (a + c);
    specificity = d / (b + d);
    falseNegative_rate = c / (a + c);
    falsePositive_rate = b / (b + d);
    predictedValuePosive = a / (a + b);
    predictedValueNegative = d / (c + d);
    falseAlarmRate = b / (a + b);
    falseReassurance = c / (c + d);
      
    fprintf(result3, '\n----- TOTAL -----\n');
	fprintf(result3, '#cp: %d\n', totalNumOfChangePoints);
    fprintf(result3, '#tp: %d\n', truePositive_total);
    fprintf(result3, '#tn: %d\n', trueNegative_total);
    fprintf(result3, '#fp: %d\n', falsePositive_total);
    fprintf(result3, '#fn: %d\n', falseNegative_total);
    fprintf(result3, '#f1-score: %f\n', f1);
    fprintf(result3, '#precision: %f\n', precision);
    fprintf(result3, '#recall: %f\n', recall);
    fprintf(result3, '\n----- OTHERS -----\n');
    fprintf(result3, 'sen: %f\n', sensitivity);
    fprintf(result3, 'spe: %f\n', specificity);
    fprintf(result3, 'fpr: %f\n', falsePositive_rate);
    fprintf(result3, 'fnr: %f\n', falseNegative_rate);
    fprintf(result3, 'pvp: %f\n', predictedValuePosive);
    fprintf(result3, 'pvn: %f\n', predictedValueNegative);
    fprintf(result3, 'far: %f\n', falseAlarmRate);
    fprintf(result3, 'fre: %f', falseReassurance);
    fclose(result3);
    
end