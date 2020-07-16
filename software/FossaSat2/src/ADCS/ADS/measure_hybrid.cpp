/* Project: FossaSat-2 Flight Software
   Author: Team Steiner
   Module: attitude_determination
   File:
   04/18/20

   This file drives the measurements hybridation algorithm
*/

/*********************** Headers ***********************/
#include "../ADCS/adcs.h"

/********************* Main function *********************/
void ADS_Measurement_Hybrid(const ADCS_CALC_TYPE v_1[ADCS_NUM_AXES], const ADCS_CALC_TYPE v_2[ADCS_NUM_AXES], const ADCS_CALC_TYPE v_3[ADCS_NUM_AXES],
                            const ADCS_CALC_TYPE m_1[ADCS_NUM_AXES], const ADCS_CALC_TYPE m_2[ADCS_NUM_AXES], const ADCS_CALC_TYPE m_3[ADCS_NUM_AXES],
                            ADCS_CALC_TYPE eulerAnglesMatrix[ADCS_NUM_AXES][ADCS_NUM_AXES]) {

  // Normalize both ephemeris and measurements vectors
  ADCS_CALC_TYPE m1[ADCS_NUM_AXES];
  memcpy(m1, m_1, ADCS_NUM_AXES*sizeof(ADCS_CALC_TYPE));
  ADCS_CALC_TYPE m2[ADCS_NUM_AXES];
  memcpy(m2, m_2, ADCS_NUM_AXES*sizeof(ADCS_CALC_TYPE));
  ADCS_CALC_TYPE m3[ADCS_NUM_AXES];
  memcpy(m3, m_3, ADCS_NUM_AXES*sizeof(ADCS_CALC_TYPE));
  ADCS_CALC_TYPE v1[ADCS_NUM_AXES];
  memcpy(v1, v_1, ADCS_NUM_AXES*sizeof(ADCS_CALC_TYPE));
  ADCS_CALC_TYPE v2[ADCS_NUM_AXES];
  memcpy(v2, v_2, ADCS_NUM_AXES*sizeof(ADCS_CALC_TYPE));
  ADCS_CALC_TYPE v3[ADCS_NUM_AXES];
  memcpy(v3, v_3, ADCS_NUM_AXES*sizeof(ADCS_CALC_TYPE));

  ADCS_CALC_TYPE m1_norm = ADCS_Add_Tolerance(ADCS_VectorNorm(m_1),0);
  ADCS_CALC_TYPE m2_norm = ADCS_Add_Tolerance(ADCS_VectorNorm(m_2),0);
  ADCS_CALC_TYPE m3_norm = ADCS_Add_Tolerance(ADCS_VectorNorm(m_3),0);

  ADCS_CALC_TYPE v1_norm = ADCS_Add_Tolerance(ADCS_VectorNorm(v_1),0);
  ADCS_CALC_TYPE v2_norm = ADCS_Add_Tolerance(ADCS_VectorNorm(v_2),0);
  ADCS_CALC_TYPE v3_norm = ADCS_Add_Tolerance(ADCS_VectorNorm(v_3),0);

  for(uint8_t i = 0; i < ADCS_NUM_AXES; i++) {
      m1[i] *= (1.0/m1_norm);
      m2[i] *= (1.0/m2_norm);
      m3[i] *= (1.0/m3_norm);

      v1[i] *= (1.0/v1_norm);
      v2[i] *= (1.0/v2_norm);
      v3[i] *= (1.0/v3_norm);
  }

  // Measurements matrix
  ADCS_CALC_TYPE measMaxtrix[ADCS_NUM_AXES][ADCS_NUM_AXES];
  measMaxtrix[0][0] = m1[0];
  measMaxtrix[0][1] = m2[0];
  measMaxtrix[0][2] = m3[0];
  measMaxtrix[1][0] = m1[1];
  measMaxtrix[1][1] = m2[1];
  measMaxtrix[1][2] = m3[1];
  measMaxtrix[2][0] = m1[2];
  measMaxtrix[2][1] = m2[2];
  measMaxtrix[2][2] = m3[2];

  // Inverse theoretical matrix
  ADCS_CALC_TYPE invV[ADCS_NUM_AXES][ADCS_NUM_AXES];
  const ADCS_CALC_TYPE det_V = (  (v1[0]*v2[1]*v3[2]) - (v1[2]*v2[1]*v3[0]) + (v1[1]*v2[2]*v3[0])
                                + (v1[2]*v2[0]*v3[1]) - (v1[1]*v2[0]*v3[2]) - (v1[0]*v2[2]*v3[1])  );

  ADCS_CALC_TYPE gain = 1.0/(ADCS_Add_Tolerance(det_V, 0));
  invV[0][0] = gain * (v2[1]*v3[2] - v2[2]*v3[1]);
  invV[0][1] = gain * (v2[2]*v3[0] - v2[0]*v3[2]);
  invV[0][2] = gain * (v2[0]*v3[1] - v2[1]*v3[0]);
  invV[1][0] = gain * (v1[2]*v3[1] - v1[1]*v3[2]);
  invV[1][1] = gain * (v1[0]*v3[2] - v1[2]*v3[0]);
  invV[1][2] = gain * (v1[1]*v3[0] - v1[0]*v3[1]);
  invV[2][0] = gain * (v1[1]*v2[2] - v1[2]*v2[1]);
  invV[2][1] = gain * (v1[2]*v2[0] - v1[0]*v2[2]);
  invV[2][2] = gain * (v1[0]*v2[1] - v1[1]*v2[0]);

  // Euler angles matrix (matrices multiplication)
  for(uint8_t i = 0; i < ADCS_NUM_AXES; i++) {
    for(uint8_t j = 0; j < ADCS_NUM_AXES; j++) {
      eulerAnglesMatrix[i][j] = 0;
      for(uint8_t k = 0; k < ADCS_NUM_AXES; k++) {
        eulerAnglesMatrix[i][j] += (measMaxtrix[i][k] * invV[k][j]);
      }
    }
  }
}