

void S2M_computeQdot( int nlhs, mxArray *plhs[],
				int nrhs, const mxArray*prhs[] ){
	// Verifier les arguments d'entrée
    checkNombreInputParametres(nrhs, 4, 4, "4 are required where the 2nd is the handler on the model, 3rd is the Q and 4th is QDot");


	// Recevoir le model
	s2mMusculoSkeletalModel * model = convertMat2Ptr<s2mMusculoSkeletalModel>(prhs[1]);
    unsigned int nQ = model->nbQ();
    unsigned int nQdot = model->nbQdot();

	// Recevoir Q
    std::vector<s2mGenCoord> Q = getParameterQ(prhs, 2, nQ);
	// Recevoir Qdot
    std::vector<s2mGenCoord> QDot = getParameterQdot(prhs, 3, nQdot);

    // S'assurer que Q et Qdot sont de la bonne dimension
    unsigned int nFrame(Q.size());

    if (QDot.size() != nFrame)
        mexErrMsgIdAndTxt( "MATLAB:dim:WrongDimension", "QDot must have the same number of frames than Q");

    /* Create a matrix for the return argument 1*/
    plhs[0] = mxCreateDoubleMatrix( nQ, nFrame, mxREAL);
    double *qdotPost = mxGetPr(plhs[0]);
    unsigned int cmp(0);

    for (unsigned int j=0; j<nFrame; ++j){
        RigidBodyDynamics::UpdateKinematicsCustom(*model, &(*(Q.begin()+j)), &(*(QDot.begin()+j)), NULL);

        // Trouver la dynamique directe a cette configuration
        s2mGenCoord QDotPost;
        model->computeQdot(*model, *(Q.begin()+j), *(QDot.begin()+j), QDotPost); // Calcul du QdotPost

        // Remplir l'output
        for (unsigned int i=0; i<nQ; i++){
            qdotPost[cmp] = QDotPost(i);
            ++cmp;
        }
    }
	return;
}
