#ifndef BIORBD_MATLAB_TAGS_JACOBIAN_H
#define BIORBD_MATLAB_TAGS_JACOBIAN_H

#include <mex.h>
#include "BiorbdModel.h"
#include "class_handle.h"
#include "processArguments.h"

void Matlab_TagsJacobian( int, mxArray *plhs[],
                                int nrhs, const mxArray*prhs[] ){
    // Verifier les arguments d'entrée
    checkNombreInputParametres(nrhs, 3, 5, "3 arguments are required [+1 optional] where the 2nd is the handler on the model and 3rd is the Q, an optional 4th if you only want technical marker [default = false] and 5th if you want to remove axes as specified in the model file [default = true]");

    // Recevoir le model
    biorbd::Model * model = convertMat2Ptr<biorbd::Model>(prhs[1]);
    unsigned int nQ = model->nbQ(); // Get the number of DoF

    // Recevoir Q
    biorbd::rigidbody::GeneralizedCoordinates Q = *getParameterQ(prhs, 2, nQ).begin();

    bool technicalTagsOnly(false);
    if (nrhs >= 4)
        technicalTagsOnly = getBool(prhs, 3);

    bool removeAxes(true);
    if (nrhs >= 5)
        removeAxes = getBool(prhs, 4);


    // Trouver la matrice jacobienne de tous les marqueurs
    std::vector<biorbd::utils::Matrix> Jac_tp;
    unsigned int nTags;
    if (technicalTagsOnly){
        Jac_tp = model->TechnicalTagsJacobian(Q, removeAxes); // Retourne la jacobienne les Tags techniques
        nTags = model->nTechTags();
    }
    else {
        Jac_tp = model->TagsJacobian(Q, removeAxes); // Retourne la jacobienne les Tags
        nTags = model->nTags();
    }
    std::vector<biorbd::utils::Matrix>::iterator it=Jac_tp.begin();

    // Create a matrix for the return argument
    plhs[0] = mxCreateDoubleMatrix( 3*nTags, nQ, mxREAL);
    double *Jac = mxGetPr(plhs[0]);
    for (unsigned int j=0; (it+j)!=Jac_tp.end(); ++j){
        for (unsigned int i=0; i<nQ; ++i){
            Jac[j+i*nTags*3] = (*(it+j))(0,i);
            Jac[j+nTags+i*nTags*3] = (*(it+j))(1,i);
            Jac[j+nTags*2+i*nTags*3] = (*(it+j))(2,i);
        }
    }

    return;
}

#endif // BIORBD_MATLAB_TAGS_JACOBIAN_H
