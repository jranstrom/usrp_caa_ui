#ifndef MAT_HF_HEADER_H
#define MAT_HF_HEADER_H

#include <matio.h>
#include <exception>

// Declare the custom namespace "matHF"
namespace matHF {

static std::vector<std::string> separate_string(const std::string & str){
	std::vector<std::string> r;
	std::stringstream sstream(str);
	std::string token;	

	while (std::getline(sstream, token, '.')) {
        r.push_back(token);
    }

    return r;
}

static mat_t* open_mat_file(const std::string & filename){
	mat_t *matfile;

	matfile = Mat_Open(filename.c_str(),MAT_ACC_RDONLY);
	if(NULL == matfile){
		throw std::runtime_error("Error opening MAT file!");
	}
	return matfile;
}

static void close_mat_file(mat_t * matfile){
	Mat_Close(matfile);
}

static matvar_t * read_base_var(mat_t * matfile,const std::string & var_name){
	matvar_t* base_mat_var = Mat_VarRead(matfile,var_name.c_str());
	if (NULL == base_mat_var){
		throw std::runtime_error("No such variable exists!");
	}

	return base_mat_var;
}

static void free_var(matvar_t * mvar){
	if(NULL != mvar){
		Mat_VarFree(mvar);
	}
}

static double read_double(const std::string & filename,const std::string & path){

	double ret_val = -1.0;

	mat_t *matfile = open_mat_file(filename);

	std::vector<std::string> n_obj = separate_string(path);

	matvar_t* var_b = read_base_var(matfile,n_obj[0]); // read base struct

	if(n_obj.size() > 1){	// var_b is a struct

		std::vector<matvar_t*> mvars;
		mvars.push_back(var_b);	
		for(size_t i=1;i<n_obj.size();i++){
			matvar_t* mvar = Mat_VarGetStructFieldByName(mvars[mvars.size()-1],n_obj[i].c_str(),0);
			mvars.push_back(mvar); // put the current struct in the list
		}

		ret_val = *(static_cast<double*>(mvars[mvars.size()-1]->data));

	}else{
		ret_val = *(static_cast<double*>(var_b->data));		
	}	

	free_var(var_b); // Mat_VarFree will loop through and delete all nested structs

	close_mat_file(matfile);

	return ret_val;
}

static std::string read_string(const std::string & filename,const std::string & path){
    std::string ret_str;

        mat_t *matfile = open_mat_file(filename);

        std::vector<std::string> n_obj = separate_string(path);

        matvar_t* var_b = read_base_var(matfile,n_obj[0]); // read base struct

        if(n_obj.size() > 1){	// var_b is a struct

            std::vector<matvar_t*> mvars;
            mvars.push_back(var_b);
            for(size_t i=1;i<n_obj.size();i++){
                matvar_t* mvar = Mat_VarGetStructFieldByName(mvars[mvars.size()-1],n_obj[i].c_str(),0);
                mvars.push_back(mvar); // put the current struct in the list
            }

            char * ret_val = (static_cast<char *>(mvars[mvars.size()-1]->data));
            ret_str = ret_val;

        }else{
            char * ret_val = (static_cast<char *>(var_b->data));
            ret_str = ret_val;
        }

        free_var(var_b); // Mat_VarFree will loop through and delete all nested structs

        close_mat_file(matfile);

    return ret_str;
}

static std::vector<double> read_double_vec(const std::string & filename,const std::string & path){
	std::vector<double> ret_vec;

	mat_t *matfile = open_mat_file(filename);

	std::vector<std::string> n_obj = separate_string(path);

	matvar_t* var_b = read_base_var(matfile,n_obj[0]); // read base struct

	if(n_obj.size() > 1){	// var_b is a struct

		std::vector<matvar_t*> mvars;
		mvars.push_back(var_b);	
		for(size_t i=1;i<n_obj.size();i++){
			matvar_t* mvar = Mat_VarGetStructFieldByName(mvars[mvars.size()-1],n_obj[i].c_str(),0);
			mvars.push_back(mvar); // put the current struct in the list
		}

		int L = std::max(mvars[mvars.size()-1]->dims[0],mvars[mvars.size()-1]->dims[1]);
		double* tmp_double_array = static_cast<double*>(mvars[mvars.size()-1]->data);

		for(size_t i=0;i<L;i++){
			ret_vec.push_back(tmp_double_array[i]);
		}

	}else{

		int L = std::max(var_b->dims[0],var_b->dims[1]);
		double* tmp_double_array = static_cast<double*>(var_b->data);

		for(size_t i=0;i<L;i++){
			ret_vec.push_back(tmp_double_array[i]);
		}
			
	}	

	free_var(var_b); // Mat_VarFree will loop through and delete all nested structs

	close_mat_file(matfile);

	return ret_vec;

}

static std::vector<std::vector<double>> read_double_mat(const std::string & filename,const std::string & path){
	std::vector<std::vector<double>> ret_mat;

	mat_t *matfile = open_mat_file(filename);

	std::vector<std::string> n_obj = separate_string(path);

	matvar_t* var_b = read_base_var(matfile,n_obj[0]); // read base struct

	if(n_obj.size() > 1){	// var_b is a struct

		std::vector<matvar_t*> mvars;
		mvars.push_back(var_b);	
		for(size_t i=1;i<n_obj.size();i++){
			matvar_t* mvar = Mat_VarGetStructFieldByName(mvars[mvars.size()-1],n_obj[i].c_str(),0);
			mvars.push_back(mvar); // put the current struct in the list
		}

		int rows = mvars[mvars.size()-1]->dims[0];
		int cols = mvars[mvars.size()-1]->dims[1];
		
		double* tmp_double_array = static_cast<double*>(mvars[mvars.size()-1]->data);


		for(size_t i=0;i<cols;i++){
			std::vector<double> c_col_vec;
			for(size_t k=0;k<rows;k++){
		 	//ret_vec.push_back(tmp_double_array[i]);
		 		 c_col_vec.push_back(tmp_double_array[i*rows+k]);
		 	}
		 	ret_mat.push_back(c_col_vec);
		}

	}else{

		int rows = var_b->dims[0];
		int cols = var_b->dims[1];
		
		double* tmp_double_array = static_cast<double*>(var_b->data);


		for(size_t i=0;i<cols;i++){
			std::vector<double> c_col_vec;
			for(size_t k=0;k<rows;k++){
		 		 c_col_vec.push_back(tmp_double_array[i*rows+k]);
		 	}
		 	ret_mat.push_back(c_col_vec);
		}

	}

	free_var(var_b);
	close_mat_file(matfile);

	return ret_mat;
}

static std::vector<int> read_as_int_vec(const std::string & filename,const std::string & path){
	std::vector<int> ret_val;

	std::vector<double> tmp_d_vec = read_double_vec(filename,path);

	for(int i = 0;i<tmp_d_vec.size();i++){
		ret_val.push_back(static_cast<int>(tmp_d_vec[i]));
	}

	return ret_val;
}

static std::vector<std::vector<int>> read_as_int_mat(const std::string & filename,const std::string & path){
	std::vector<std::vector<int>> ret_mat;

	std::vector<std::vector<double>> tmp_d_mat = read_double_mat(filename,path);

	for(size_t k=0;k<tmp_d_mat.size();k++){
		std::vector<int> tmp_col_vec;
		for(size_t i=0;i<(tmp_d_mat[k]).size();i++){
			tmp_col_vec.push_back(static_cast<int>((tmp_d_mat[k])[i]));
		}
		ret_mat.push_back(tmp_col_vec);
	}

	return ret_mat;
}

static int read_as_int(const std::string & filename,const std::string & path){
	return static_cast<int>(read_double(filename, path));
}

static std::complex<double> read_complex_double(const std::string & filename,const std::string & path){
	
	double real_v = 0;
	double imag_v = 0;

	mat_t *matfile = open_mat_file(filename);

	std::vector<std::string> n_obj = separate_string(path);

	matvar_t* var_b = read_base_var(matfile,n_obj[0]); // read base struct

	if(n_obj.size() > 1){	// var_b is a struct

		std::vector<matvar_t*> mvars;
		mvars.push_back(var_b);	
		for(size_t i=1;i<n_obj.size();i++){
			matvar_t* mvar = Mat_VarGetStructFieldByName(mvars[mvars.size()-1],n_obj[i].c_str(),0);
			mvars.push_back(mvar); // put the current struct in the list
		}
		if(mvars[mvars.size()-1]->isComplex){
			mat_complex_split_t *complex_data = (mat_complex_split_t *)mvars[mvars.size()-1]->data;
		
			real_v = *(static_cast<double*>(complex_data->Re));
			imag_v = *(static_cast<double*>(complex_data->Im));
		}
	}else{
		if(var_b->isComplex){
			mat_complex_split_t *complex_data = (mat_complex_split_t *)var_b->data;
		
			real_v = *(static_cast<double*>(complex_data->Re));
			imag_v = *(static_cast<double*>(complex_data->Im));
		}	
	}	

	free_var(var_b); // Mat_VarFree will loop through and delete all nested structs

	close_mat_file(matfile);

	std::complex<double> ret_val(real_v,imag_v);

	return ret_val;
}

static std::vector<std::complex<double>> read_complex_double_vec(const std::string & filename,const std::string & path){
	std::vector<std::complex<double>> ret_val;


	mat_t *matfile = open_mat_file(filename);

	std::vector<std::string> n_obj = separate_string(path);

	matvar_t* var_b = read_base_var(matfile,n_obj[0]); // read base struct

	if(n_obj.size() > 1){	// var_b is a struct

		std::vector<matvar_t*> mvars;
		mvars.push_back(var_b);	
		for(size_t i=1;i<n_obj.size();i++){
			matvar_t* mvar = Mat_VarGetStructFieldByName(mvars[mvars.size()-1],n_obj[i].c_str(),0);
			mvars.push_back(mvar); // put the current struct in the list
		}
		if(mvars[mvars.size()-1]->isComplex){
			//std::cout << "complex array [" << mvars[mvars.size()-1]->dims[0] << " x " << mvars[mvars.size()-1]->dims[1] << "]" << std::endl;

			int L = std::max(mvars[mvars.size()-1]->dims[0],mvars[mvars.size()-1]->dims[1]);
			mat_complex_split_t *complex_data = (mat_complex_split_t *)mvars[mvars.size()-1]->data;

			double* tmp_double_a_real = static_cast<double*>(complex_data->Re);
			double* tmp_double_a_imag = static_cast<double*>(complex_data->Im);
			for (size_t i =0;i<L;i++){
				ret_val.push_back(std::complex<double>(tmp_double_a_real[i],tmp_double_a_imag[i]));
			}

		}		

	}else{

		if(var_b->isComplex){
			//std::cout << "complex array [" << var_b->dims[0] << " x " << var_b->dims[1] << "]" << std::endl;

			int L = std::max(var_b->dims[0],var_b->dims[1]);
			mat_complex_split_t *complex_data = (mat_complex_split_t *)var_b->data;

			double* tmp_double_a_real = static_cast<double*>(complex_data->Re);
			double* tmp_double_a_imag = static_cast<double*>(complex_data->Im);
			for (size_t i =0;i<L;i++){
				ret_val.push_back(std::complex<double>(tmp_double_a_real[i],tmp_double_a_imag[i]));
			}

		}	

	}	

	free_var(var_b); // Mat_VarFree will loop through and delete all nested structs

	close_mat_file(matfile);

	return ret_val;
}

static std::vector<char> read_single_char_cell_array(const std::string & filename,const std::string & path){
	std::vector<char> ret_val;

	mat_t *matfile = open_mat_file(filename);

	std::vector<std::string> n_obj = separate_string(path);

	matvar_t* var_b = read_base_var(matfile,n_obj[0]); // read base struct

	if(n_obj.size() > 1){	// var_b is a struct

		std::vector<matvar_t*> mvars;
		mvars.push_back(var_b);	
		for(size_t i=1;i<n_obj.size();i++){
			matvar_t* mvar = Mat_VarGetStructFieldByName(mvars[mvars.size()-1],n_obj[i].c_str(),0);
			mvars.push_back(mvar); // put the current struct in the list
		}

		int L = std::max(mvars[mvars.size()-1]->dims[0],mvars[mvars.size()-1]->dims[1]);
		for(size_t i=0;i<L;i++){
			matvar_t* cell_element = Mat_VarGetCell(mvars[mvars.size()-1],i);
			char* str = (static_cast<char*>(cell_element->data));
			ret_val.push_back(str[0]);
		}

	}else{

		int L = std::max(var_b->dims[0],var_b->dims[1]);
		for(size_t i=0;i<L;i++){
			matvar_t* cell_element = Mat_VarGetCell(var_b,i);
			char* str = (static_cast<char*>(cell_element->data));
			ret_val.push_back(str[0]);
		}
			
	}	

	free_var(var_b); // Mat_VarFree will loop through and delete all nested structs

	close_mat_file(matfile);

	return ret_val;
}

}

#endif
