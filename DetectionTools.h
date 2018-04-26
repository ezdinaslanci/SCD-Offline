#include <armadillo>
#include <fstream>
#include <iostream>

using namespace std;
using namespace arma;

int mod(int a, int b){
    int m = a % b;
    if(m < 0)
        m += b;
    return m;
}

int writeMatrixToFile (mat m, string fileName, char delimiter, int precision) {
    ofstream output(fileName);
    output.setf( std::ios::fixed, std:: ios::floatfield );
    output.precision(precision);
    for (int i = 0; i < m.n_rows; i++) {
        for (int j = 0; j < m.n_cols; j++) {
            output << m(i,j) << delimiter;
        }
        output << "\n";
    }
    return 1;
}
int writeChangePoints(string fileName, string expName, mat changePoints, int startPoint){
    ofstream output(fileName, ofstream::app);
    output.setf( std::ios::fixed);
    output << expName <<":";
    for (int i = 0; i < changePoints.n_cols; i++){
        if (int(changePoints(i)) > startPoint) 
			output << ' ' << int(changePoints(i));
	}
    output << '\n';
}

mat findPeaks (mat data, int startPoint, int minPeakDist, double minPeakHeight) {
    int r = data.n_rows;
    mat maxPoints(1, r, fill::zeros);
    int indexOfMax = 0;
    int numOfMax = 0;
    while (1) {
        indexOfMax = data.index_max();
        if(data(indexOfMax) < minPeakHeight) {
            break;
        }
        if(indexOfMax >= startPoint){
            maxPoints(numOfMax) = indexOfMax;
            numOfMax++;
        }
        if(indexOfMax - minPeakDist <= 1)
            data.rows(0, indexOfMax + minPeakDist).fill(0);
        else if(indexOfMax + minPeakDist >= r - 2)
            data.rows(indexOfMax - minPeakDist + 1, r - 1).fill(0);
        else
            data.rows(indexOfMax - minPeakDist, indexOfMax + minPeakDist).fill(0);
    }
    maxPoints.resize(1, numOfMax);
    return trans(sort(trans(maxPoints)));
}

mat avgOfLargestN (mat data, int n) {

	int r = data.n_rows;
    int c = data.n_cols;
	
	int ln = n;
	if(n == 0){
		ln = round((c*3)/32);
	}
    mat avgOf(r, 1, fill::zeros);
    data = trans(sort(trans(data)));
    avgOf = trans(sum(trans(data.cols(c - ln, c - 1))))/ln;
	avgOf = avgOf / mean(mean(avgOf));
    return avgOf;
}

mat dynamismAmplifier(mat data, int look_buffer, int look_direction, int start_point, double delta){ //look_direction -> 1:left 2:right 3:both
	// if delte is zero linearly decrease otherwise exponentially decrease 
    int r = data.n_rows;
    int c = data.n_cols;
    if (start_point >= r){
        start_point = 1;
        cerr << "\nStart Point Error !!\n";
        return data;
    }
    vec pen = linspace <vec> (1, 0, look_buffer);
    mat dyna(r, c, fill::zeros);
    for (int i = start_point; i < r; i++) {
        vec total (c, fill::zeros);
        int count = 0;
        if (look_direction == 1 || look_direction == 3) {
            for (int k = i - 1; (k > i - 1 - look_buffer) && (k < r) && k >= 0; k--) {
				if (delta == 0)
					total += trans(abs(data.row(k) - data.row(i)) * pen(i - k - 1));
				else
					total += trans(abs(data.row(k) - data.row(i)) * pow(delta, i - k - 1));
                count++;
            }
        }
        if(look_direction == 2 || look_direction == 3)
            for (int k = i + 1; (k < i + 1 + look_buffer) && (k >= 0) && (k < r); k++) {
				if (delta == 0)
					total += trans(abs(data.row(k) - data.row(i)) * pen(k - i - 1));
				else
					total += trans(abs(data.row(k) - data.row(i)) * pow(delta, k - i - 1));
                count++;
            }
        dyna.row(i) = trans(total / (count + 1));
    }
    for(int i = 0; i< start_point;i++){
        dyna.row(i) = dyna.row(start_point);
    }
    return dyna;
}

mat movingMean(mat data, int windowSize){
    if (windowSize <= 1){
        return data;
    }
    int row = data.n_rows;
    int col = data.n_cols;
    mat cm0_smooth(row, col, fill::zeros);
    for (int j = 0; j < col; j++){
        for (int i = 0; i< row; i++){
            if(i < windowSize)
                cm0_smooth(i, j) = mean(data(span(0,i), j));
            else
                cm0_smooth(i, j) = mean(data(span(i-windowSize + 1,i), j));

        }
    }

    return cm0_smooth;
}
mat SLWEVOMCProbabilityEstimator(string inputSequenceFileName, double lambda, int depth, char markovSwitch [], char normalize){
    int mntl = 0, mxtl = 2;
    ivec seq;   seq.load(inputSequenceFileName);
    ivec u = unique(seq);
    int alph = u.n_elem;
    int cc = seq.size();
    mat k0(cc, pow(alph, 1), fill:: zeros);
    mat k1(cc, pow(alph, 2), fill:: zeros);
    mat k2(cc, pow(alph, 3), fill:: zeros);
    vec P0(pow(alph, 1), fill::ones);  P0 = P0 * (1 / pow(alph, 1));
    vec P1(pow(alph, 2), fill::ones);  P1 = P1 * (1 / pow(alph, 2));
    vec P2(pow(alph, 3), fill::ones);  P2 = P2 * (1 / pow(alph, 3));
    for(int n = 0; n < cc; n++ ) {
        int i0 = seq[n] - 1;
        int i1 = alph * (seq[mod(n + 1, cc)] - 1) + seq[n] - 1;
        int i2 = alph * alph*(seq[mod(n + 2, cc)] - 1) + alph * (seq[mod(n + 1, cc)] - 1) + seq[n] - 1;
        double temp0 = P0(i0);             double temp1 = P1(i1);             double temp2 = P2(i2);
        double sumP0 = sum(P0) - P0(i0);   double sumP1 = sum(P1) - P1(i1);   double sumP2 = sum(P2) - P2(i2);
        P0 = lambda * P0;                  P1 = lambda * P1;                  P2 = lambda * P2;
        P0(i0) = temp0 + sumP0 * (1 - lambda);
        P1(i1) = temp1 + sumP1 * (1 - lambda);
        P2(i2) = temp2 + sumP1 * (1 - lambda);
        k0.row(n) = trans(P0);
        k1.row(n) = trans(P1);
        k2.row(n) = trans(P2);
    }



    //SLWEMeanvar
    mat mu0(cc, alph, fill::zeros);   mat mu1(cc, pow(alph, 2), fill::zeros);   mat mu2(cc, pow(alph, 3), fill::zeros);
    mu0.row(0) = k0.row(0); mu1.row(0) = k1.row(0); mu2.row(0) = k2.row(0);
    for(int i = 1; i < cc; i++){
        if(i - depth < -1){
            mu0.row(i) = (mu0.row(i - 1) * mod(i, depth) + k0.row(i)) / (mod(i+1, depth));
            mu1.row(i) = (mu1.row(i - 1) * mod(i, depth) + k1.row(i)) / (mod(i+1, depth));
            mu2.row(i) = (mu2.row(i - 1) * mod(i, depth) + k2.row(i)) / (mod(i+1, depth));
        }

        else if(i - depth == -1){
            mu0.row(i) = (mu0.row(i - 1) * (depth - 1) + k0.row(i)) / depth;
            mu1.row(i) = (mu1.row(i - 1) * (depth - 1) + k1.row(i)) / depth;
            mu2.row(i) = (mu2.row(i - 1) * (depth - 1) + k2.row(i)) / depth;
        }

        else{
            mu0.row(i) = (mu0.row(i - 1) * depth + k0.row(i) - k0.row(i - depth)) / depth;
            mu1.row(i) = (mu1.row(i - 1) * depth + k1.row(i) - k1.row(i - depth)) / depth;
            mu2.row(i) = (mu2.row(i - 1) * depth + k2.row(i) - k2.row(i - depth)) / depth;
        }

    }


    //tripletvar
    mat cm0(cc, pow(alph, 2), fill::zeros);
    mat qm1(cc, pow(alph, 2), fill::zeros);
	mat markovDeps (cc, pow(alph, 2)*2 + alph, fill::zeros);
    for (int j = 1; j <= alph; j++) {
        for(int i = 1; i <= alph; i++){
            cm0.col(alph * (j - 1) + i - 1)  = mu1.col(alph * (i - 1) + j - 1) / mu0.col(j-1);
            vec mu1totol(cc, fill::zeros);
            for(int k = 1; k <= alph; k++){
                qm1.col(alph * (j - 1) + i - 1) += mu2.col(alph * alph*(i - 1) + alph * (k - 1) + j - 1);
                mu1totol += mu1.col(alph * (k - 1) + j - 1);
            }
            qm1.col(alph * (j - 1) + i - 1)= qm1.col(alph * (j - 1) + i - 1) / mu1totol;
        }
    }

	if(markovSwitch[1] != '0')
		markovDeps.cols(0, pow(alph, 2)- 1) = cm0;

	if(markovSwitch[2] != '0')
		markovDeps.cols(pow(alph, 2), pow(alph, 2) * 2 - 1) = qm1;
	
	if(normalize != '0')
		markovDeps = markovDeps/alph;
	
	if(markovSwitch[0] != '0')
		markovDeps.cols(pow(alph, 2) * 2, pow(alph, 2) * 2 + alph - 1) = mu0;
	
    return markovDeps;
}