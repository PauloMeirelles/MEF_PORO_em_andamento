#pragma hdrstop
#pragma argsused


#include <iostream>
#include "main.h"
#include <math.h>

//#ifdef _WIN32
#include <tchar.h>
//#else
typedef char _TCHAR;
#define _tmain main
//#endif

#include "main.h"

int _tmain(int argc, _TCHAR* argv[])
{


    // insert code here...
    //int main (int argc, const char * argv[])
	//{
        int k=0;
		std::string EndCON="E:/-----MESTRADO_IMPORTANTE-----/CODIGOS/outros/08_12_2021/MEF_PORO_ATT_WILSON/PFEM_Poro_Atual/config.txt";
		std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(7);
		tvNo No;
		tvEl El;
        tvMa Ma;
		tvQuadratura Qua;
		CCo Co(EndCON);
        int ngl;
		//Apaga os arquivos a serem preenchidos com os relatorios...
		std::string NArq1; NArq1.append(Co.r_ArqREL()); //NArq1.append("resultados/Saida_Triang.txt");
		std::ofstream Sai1;   Sai1.open(NArq1.c_str()); ///< número de nós e matriz a ser invertida
		Sai1.close();
		Le_Quadraturas(Qua,Co.r_ArqQUA());
		std::cout << Co.r_ArqNOS() << std::endl;
		Le_Nos(No,Co.r_ArqNOS());
		Le_CC(No,Co.r_ArqCC(),ngl);
        Le_P(No,Co.r_ArqP());
		Ma =Le_Mat(Co.r_ArqMAT());
        Le_Elementos(El,No,Ma,Qua,Co);
		int NNQ=0;
		for (int i=0; i<El.size(); i++)
        {
			NNQ+=El[i].r_Quad()->r_NumberQuad();
		}
		int NNP=0;  //!!!!!!!cuidado esta vaiavel e tudo que ela implica tem que ser seriamente repensada
        /// especialmente para os casos em que existem nós declarados que nao sao usados na estrutura....
        for (int i=0; i<No.size(); i++)
        {
			NNP+=No[i].r_FP();
        }
        std::cout << NNQ << " " << NNP << std::endl;
        int nj=0;
		V2D MPHI;
        MPHI.resize(NNQ,V1D (NNP,0.0));
        for (int i=0; i<El.size(); i++)
        {
			for (int j=0; j<El[i].r_Quad()->r_NumberQuad(); j++)
			{
				for (int k=0; k<El[i].r_NNo(); k++)
                {
					MPHI[nj][El[i].r_No(k)->r_NumberNo()]=El[i].r_PHI(j,k);
				}
				nj=nj+1;
            }
		}
		V2D iMPHI;
		iMPHI.resize(MPHI[0].size(),V1D (MPHI.size(),0.0));
		iMPHI=inverse(MPHI);  //www
        std::vector< std::vector <double> > Fint;    Fint.resize(ngl,std::vector<double>(1,0.0));
        std::vector< std::vector <double> > Fext;    Fext.resize(ngl,std::vector<double>(1,0.0));
		std::vector< double > _g;       _g.resize(ngl,0.0);
		std::vector< double > Dy;      Dy.resize(ngl,0.0);
		std::vector< std::vector< double > > H;      H.resize(ngl,std::vector<double> (ngl,0.0));
		std::vector< std::vector< double > > M;      M.resize(ngl,std::vector<double> (ngl,0.0));


        double Nx;
		double NDy;
        Nx=0;
		for (int i=0; i<No.size(); i++) { for (int j=0; j<3; j++) { if (No[i].r_CondCountour(j)==0) { Nx+=pow(No[i].r_X(0,j),2.0); } } }
        Nx=pow(Nx,0.5);
		double Y;
        int ite;
        ite=0;
		do
		{

			for (int i=0; i<El.size(); i++)
			{
				El[i].ite();
				for (int a=0; a<El[i].r_NNo(); a++)
				{
					for (int z=0; z<2; z++)
					{


/*
                        InteiraFint[2*El[i].r_No(a)->r_I()+z][0]+=El[i].r_Fint(2*a+z);
                        InteiraFext[2*El[i].r_No(a)->r_I()+z][0] =El[i].r_No(a)->r_P(z);
						Inteira_g  [2*El[i].r_No(a)->r_I()+z]    =InteiraFext[2*El[i].r_No(a)->r_I()+z][0]-InteiraFint[El[i].r_No(a)->r_I()+z][0];
                        for (int l=0; l<El[i].r_NN(); l++)
                        {
                            for (int k=0; k<2; k++)
							{
                                    InteiraH[2*El[i].r_No(a)->r_I()+z][2*El[i].r_No(l)->r_I()+k]+=El[i].r_H(2*El[i].r_d()*a+z,2*El[i].r_d()*l+k);
                            }
                        }
*/


						if (El[i].r_No(a)->r_Adress(z)>=0)
						{
							Fint[El[i].r_No(a)->r_Adress(z)][0]+=El[i].r_Fint(El[i].r_dimension()*a+z);
							Fext[El[i].r_No(a)->r_Adress(z)][0] =El[i].r_No(a)->r_PConcentrated(z);
							_g[El[i].r_No(a)->r_Adress(z)] =Fext[El[i].r_No(a)->r_Adress(z)][0]-Fint[El[i].r_No(a)->r_Adress(z)][0];
							for (int l=0; l<El[i].r_NNo(); l++)
                            {
                                for (int k=0; k<2; k++)
                                {
									if (El[i].r_No(l)->r_Adress(k)>=0)
                                    {
										H[El[i].r_No(a)->r_Adress(z)][El[i].r_No(l)->r_Adress(k)]+=El[i].r_Hessiana(El[i].r_dimension()*a+z,El[i].r_dimension()*l+k);
										M[El[i].r_No(a)->r_Adress(z)][El[i].r_No(l)->r_Adress(k)]+=El[i].r_Mass(El[i].r_dimension()*a+z,El[i].r_dimension()*l+k);
									}
                                }
                            }
                        }
					}
                }
            }

		/*    std::cout << "Matriz H Iteracao=" << ite << std::endl;
            std::cout << "MatrixForm[HH={";
            for (int i=0; i<H.size(); i++)
            {
				std::cout << "{";
                for (int k=0; k<H[0].size(); k++)
                {
                    std::cout << H[i][k];
					if (k<H[0].size()-1) std::cout << ",";
                }
                std::cout << "}";
                if (i<H.size()-1) std::cout << ",";
			}
            std::cout << std::endl;
         */

           std::cout << "Matriz M Iteracao=" << ite << std::endl;
		   std::cout << "MatrixForm[MM={";
           for (int i=0; i<M.size(); i++)
           {
           std::cout << "{";
           for (int k=0; k<M[0].size(); k++)
		   {
           std::cout << M[i][k];
           if (k<M[0].size()-1) std::cout << ",";
           }
		   std::cout << "}";
           if (i<M.size()-1) std::cout << ",";
           }
            std::cout << "}]";
			std::cout << std::endl;

            /*
            std::cout << "Vetor F Iteracao=" << ite << std::endl;
			std::cout << "MatrixForm[FF={";
            for (int i=0; i<_g.size(); i++)
            {
                std::cout << _g[i];
                if (i<_g.size()-1) std::cout << ",";
			}
            std::cout << "}]";
            std::cout << std::endl;
             */















            Sai1.open(NArq1.c_str(),std::fstream::in | std::fstream::out | std::fstream::app);     ///< Dados Calculados

			std::vector< std::vector< double > > Hi;
			Hi.resize(ngl,std::vector<double> (ngl,0.0));
			Hi=inverse(H);
			Dy=MV(Hi,_g);
			//RESOLVE_GAUSS(Dy,H,_g);    //www
		   // Dy=LAPresolve(H,_g);

            std::cout << std::endl;
            std::cout << std::endl;
			std::cout << "#####################" << std::endl;
            for (int i=0; i<ngl; i++)
            {
                std::cout << Dy[i] << " ";
            }
            std::cout << std::endl;
			std::cout << std::endl;

            NDy=0;
            for (int i=0; i<Dy.size(); i++) { NDy+=pow(Dy[i],2.0); }
            NDy=pow(NDy,0.5);
            Sai1 << " Dy = ";
			for (int i=0; i<Dy.size(); i++) { Sai1 << Dy[i] << " "; }
            Sai1 << std::endl;
            Sai1.close();
            for (int i=0; i<ngl; i++)
            {
                Fint[i][0]=0.0;
				for (int j=0; j<ngl; j++)
                {
                    H[i][j]=0.0;
                    M[i][j]=0.0;
				}
            }
			k=0;
            for (int i=0; i<No.size(); i++)
            {
                for (int j=0; j<2; j++)
                {
					if (No[i].r_CondCountour(j)==0)
					{
						k=k+1;    ///++K atualisa antes k++ atualisa depois;
						Y=No[i].r_X(1,j)+Dy[k-1];
                        No[i].w_X(1,j,Y);
                    }
                }
				// std::cout << i << " " << No[i].r_X(1,0) << " " << No[i].r_X(1,1) << " " << No[i].r_X(1,2) << std::endl;
            }
            std::cout << "passo1" << std::endl;

            for (int k1=0; k1<2; k1++)
            {
				for (int k2=0; k2<2; k2++)
                {
                    V1D GRA; GRA.resize(NNQ,0.0);
                    V1D GRB; GRB.resize(NNP,0.0);
                    int a=0;
                    for (int i=0; i<El.size(); i++)
					{
						for (int j=0; j<El[i].r_Quad()->r_NumberQuad(); j++)
                        {
                            GRA[a]=El[i].r_E(j,2*k1+k2);
                            a=a+1;
                        }
					}
                    GRB=MV(iMPHI,GRA);
                    for (int i=0; i<No.size(); i++)
                    {
                        No[i].w_no_E(2*k1+k2,GRB[i]);
                    }
					for (int i=0; i<El.size(); i++)
                    {
						for (int j=0; j<El[i].r_NNo(); j++)
                        {
                            El[i].w_no_E(j,2*k1+k2,El[i].r_No(j)->r_no_E(2*k1+k2));
                        }
					}
                }
            }
            for (int k1=0; k1<2; k1++)
            {
                for (int k2=0; k2<2; k2++)
				{
                    V1D GRA; GRA.resize(NNQ,0.0);
                    V1D GRB; GRB.resize(NNP,0.0);
                    int a=0;
                    for (int i=0; i<El.size(); i++)
                    {
						for (int j=0; j<El[i].r_Quad()->r_NumberQuad(); j++)
                        {
                            GRA[a]=El[i].r_S(j,2*k1+k2);
                            a=a+1;
                        }
                    }
					GRB=MV(iMPHI,GRA);
                    for (int i=0; i<No.size(); i++)
                    {
                        No[i].w_no_S(2*k1+k2,GRB[i]);
                    }
                    for (int i=0; i<El.size(); i++)
					{
						for (int j=0; j<El[i].r_NNo(); j++)
                        {
                            El[i].w_no_S(j,2*k1+k2,El[i].r_No(j)->r_no_S(2*k1+k2));
                        }
                    }
                }
			}
            V1D GRAa; GRAa.resize(NNQ,0.0);
            V1D GRBa; GRBa.resize(NNP,0.0);
            int aa=0;
            for (int i=0; i<El.size(); i++)
            {
				for (int j=0; j<El[i].r_Quad()->r_NumberQuad(); j++)
				{
                    GRAa[aa]=El[i].r_ue(j);
                    aa=aa+1;
                }
            }
            GRBa=MV(iMPHI,GRAa);
            for (int i=0; i<No.size(); i++)
			{
                No[i].w_no_ue(GRBa[i]);
            }
            for (int i=0; i<El.size(); i++)
            {
				for (int j=0; j<El[i].r_NNo(); j++)
                {
					El[i].w_no_ue(j,El[i].r_No(j)->r_no_ue());
				}
			}
			std::cout << "passo2" << std::endl;

			Gera_VTK_69(Co,No,El,ite);
			std::cout << "passo3" << std::endl;
			std::cout << ite << std::endl;
			ite+=1;


		}
		while (NDy/Nx>0.00001) ;
		std::cout << "Fim" << std::endl;
        return 0;
	//}
}



void Gera_VTK_69(CCo& Co, tvNo& No, tvEl& El, const int& ite)
{
    std::vector < std::vector < int > > ind;
    ind.resize(2, std::vector < int > (2,0.0));
    ind[0][0]=0; ind[0][1]=2;
    ind[1][0]=3; ind[1][1]=1;
    std::string NArq2; NArq2.append(Co.r_ArqVTK());
    NArq2.insert(NArq2.find(".vtk"),intToString(ite));
    std::ofstream Sai2; Sai2.open(NArq2.c_str()); Sai2.close();
    Sai2.open(NArq2.c_str(),std::fstream::in | std::fstream::out | std::fstream::app);     ///< Dados Calculado
    //  int NEL; NEL=0;
    //  int NNT; NNT=0;
    //  for (int i=0; i<El.size(); i++)
    //  {
    //    NEL+=El[i].NTriL(  El[i].r_Pe()  );
    //    NNT+=El[i].r_NN();
    //  }
    Sai2 << "# vtk DataFile Version 2.0" << std::endl;
    Sai2 << "FEM, Created by Wutzow" << std::endl;
    Sai2 << "ASCII" << std::endl;
    Sai2 << "DATASET UNSTRUCTURED_GRID" << std::endl;
    Sai2 << "POINTS " << No.size() << " Float64" << std::endl;
    for (int i=0; i<No.size(); i++)
    {
        Sai2 << No[i].r_X(1,0) << " " << No[i].r_X(1,1) << " " << 0.0 << std::endl;
    }
    Sai2 << std::endl;
    Sai2 << "CELLS " << El.size() << " " << El.size()*11 << std::endl;
    for (int i=0; i<El.size(); i++)
    {
        Sai2 << "10";
		Sai2 << " " << El[i].r_No(0)->r_NumberNo();
		Sai2 << " " << El[i].r_No(6)->r_NumberNo();
		Sai2 << " " << El[i].r_No(9)->r_NumberNo();
		Sai2 << " " << El[i].r_No(1)->r_NumberNo();
		Sai2 << " " << El[i].r_No(3)->r_NumberNo();
		Sai2 << " " << El[i].r_No(7)->r_NumberNo();
		Sai2 << " " << El[i].r_No(8)->r_NumberNo();
		Sai2 << " " << El[i].r_No(5)->r_NumberNo();
		Sai2 << " " << El[i].r_No(2)->r_NumberNo();
		Sai2 << " " << El[i].r_No(4)->r_NumberNo();
        Sai2 << std::endl;
    }
    Sai2 << std::endl;
    Sai2 << "CELL_TYPES " << El.size() << std::endl;
    for (int i=0; i<El.size(); i++)
    {
        Sai2 << "69" << std::endl;
    }
    Sai2 << std::endl;
    Sai2 << "CELL_DATA "<< El.size() << std::endl;
    Sai2 << "POINT_DATA " << No.size() << std::endl;
    Sai2 << "VECTORS Y" <<  " Float64" << std::endl;
    for (int i=0; i<No.size(); i++)
    {
        Sai2 << No[i].r_X(1,0) << " " << No[i].r_X(1,1) << " 0.0" << std::endl;
    }
    Sai2 << std::endl;
    Sai2 << "CELL_DATA "<< El.size() << std::endl;
    Sai2 << "POINT_DATA " << No.size() << std::endl;
    Sai2 << "VECTORS U" << " Float64" << std::endl;
    for (int i=0; i<No.size(); i++)
    {
        Sai2 << No[i].r_X(1,0)-No[i].r_X(0,0) << " " << No[i].r_X(1,1)-No[i].r_X(0,1) << " 0.0" << std::endl;
    }
    Sai2 << std::endl;

     Sai2 << "CELL_DATA "<< El.size() << std::endl;
     Sai2 << "POINT_DATA " << No.size() << std::endl;
     Sai2 << "TENSORS E Float64" << std::endl;
     for (int i=0; i<No.size(); i++)
     {
       Sai2 << No[i].r_no_E(ind[0][0]) << " " << No[i].r_no_E(ind[0][1]) << " " << " 0.00" << std::endl;
       Sai2 << No[i].r_no_E(ind[1][0]) << " " << No[i].r_no_E(ind[1][1]) << " " << " 0.00" << std::endl;
       Sai2 << "0.00"                  << " " << "0.00"                  << " " << " 0.00" << std::endl;
       Sai2 << std::endl;
     }
     Sai2 << std::endl;

    Sai2 << "CELL_DATA "<< El.size() << std::endl;
    Sai2 << "POINT_DATA " << No.size() << std::endl;
    Sai2 << "TENSORS S Float64" << std::endl;
    for (int i=0; i<No.size(); i++)
    {
        Sai2 << No[i].r_no_S(ind[0][0]) << " " << No[i].r_no_S(ind[0][1]) << " " << " 0.00" << std::endl;
        Sai2 << No[i].r_no_S(ind[1][0]) << " " << No[i].r_no_S(ind[1][1]) << " " << " 0.00" << std::endl;
        Sai2 << "0.00"                  << " " << "0.00"                  << " " << " 0.00" << std::endl;
        Sai2 << std::endl;
    }
    Sai2 << std::endl;

     Sai2 << "CELL_DATA "<< El.size() << std::endl;
     Sai2 << "POINT_DATA " << No.size() << std::endl;
     Sai2 << "SCALARS ue float" << std::endl;
     Sai2 << "LOOKUP_TABLE default" << std::endl;
     for (int i=0; i<No.size(); i++)
     {
     Sai2 << No[i].r_no_ue() << std::endl;
     }
    Sai2 << std::endl;
    Sai2.close();
}
