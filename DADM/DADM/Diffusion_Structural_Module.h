#pragma once
#include "MRI_Module.h"
typedef enum DataType {STRUCTURAL_DATA, DIFFUSION_DATA} DataType;

template <class InputDataType, class OutputDataType>
class Diffusion_Structural_Module:
	public MRI_Module<InputDataType, OutputDataType>
{
public:
	Diffusion_Structural_Module() {};
	virtual void Start();
	virtual ~Diffusion_Structural_Module() {};

protected:
	DataType dtype;

private:
	virtual void StructuralDataAlgorithm() = 0;
	virtual void DiffusionDataAlgorithm() = 0;
};

