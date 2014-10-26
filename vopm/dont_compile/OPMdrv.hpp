#ifndef __OPMDRV__
#include <list>
//#include "aeffectx.h"
using namespace std;

#define CHMAX 8
#define VOMAX 128

#define ERR_NOERR 0x0000
#define ERR_NOFILE 0x10000000
#define ERR_SYNTAX 0x20000000
#define ERR_LESPRM 0x30000000

#define CHSLOT8 15 //Midi 15Ch assigned OPM8ch

class OPDATA{
	public:
	 //EG-Level
		unsigned int TL;
		unsigned int D1L;
	 //EG-ADR
		unsigned int AR;
		unsigned int D1R;
		unsigned int D2R;
		unsigned int RR;
	 //DeTune
		unsigned int KS;
		unsigned int DT1;
		unsigned int MUL;
		unsigned int DT2;
		unsigned int F_AME;

		OPDATA(void);
};

class CHDATA{
	public:
		unsigned int F_PAN;//L:0x80Msk R:0x40Msk
		unsigned int CON;
		unsigned int FL;
		unsigned int AMS;
		unsigned int PMS;
		unsigned int F_NE;
		unsigned int OpMsk;
		class OPDATA Op[4];
		char Name[16];
		unsigned int SPD;
		unsigned int PMD;
		unsigned int AMD;
		unsigned int WF;
		unsigned int NFQ;
		CHDATA(void);
// ~CHDATA(void);

};


struct TBLMIDICH{
	int VoNum;				//���F�ԍ� 0-
	int Bend;				//�s�b�`�x���h -8192�`+8191[signed 14bit]
	int Vol;				//�{�����[�� 0-127
	int Pan;				//L:0x80 Msk,R:0x40 Msk
	int BendRang;			//�s�b�`�x���h�����W�@1-??[�����̔{��]
	int PMD;
	int AMD;
	int SPD;
	int OpTL[4];			//OpTL�̑��Εω�
	unsigned int RPN;
	unsigned int NRPN;
	bool CuRPN;
	int BfNote;				//�O��̃m�[�g�ԍ��i�|���^�����g�p�j
	int PortaSpeed;			//�|���^�����g�X�s�[�h
	bool PortaOnOff;
	int LFOdelay;
};





class OPMDRV{
// friend class VOPM;
private:
	unsigned int SampleTime;	//HOST����w�肳�ꂽDelta����
	char CuProgName[64];		//����HOST�ɑI������Ă���v���O������
	int CuProgNum;				//����HOST�ɑI������Ă���v���O�����ԍ�

	int NoisePrm;				//OPM �m�C�Y�p�����[�^
	int MstVol;					//�}�X�^�[�{�����[��
	bool PolyMono;				//Poly true,Mono false
	class Opm *pOPM;
	int OpmClock;				//OPM�̃N���b�N 0:3.58MHz 1:4MHz
//����CH�Ǘ��o�������X�g
	list<int> PlayCh,WaitCh;
	
	struct {
		int VoNum;				//���F�ԍ�0-
		int MidiCh;				//Midi�`�����l��
		int Note;				//�m�[�g�ԍ�(0-127,���g�p��255)
		int Vel;				//�x���V�e�B
		int PortaBend;			//�|���^�����g�p�̃x���h�I�t�Z�b�g
		int PortaSpeed;
		int LFOdelayCnt;
	}TblCh[CHMAX];

	struct TBLMIDICH TblMidiCh[16];	//Midi�`�����l���ݒ�e�[�u��

	int AtachCh(void);			//�`�����l���ǉ�
	void DelCh(int);			//�`�����l���폜
	int ChkEnCh(void);			//�󂫃`�����l����Ԃ��B
	int ChkSonCh(int,int);		//MidiCh�ƃx���V�e�B��蔭�����̃`�����l����T���B
	int NonCh(int,int,int);		//MidiCh�ƃm�[�g�A�x���V�e�B�Ŕ�������B
	int NoffCh(int,int);		//MidiCh�ƃm�[�g�ŏ�������B

	void SendVo(int);			//���F�p�����[�^��OPM�ɑ���
	void SendKc(int);			//���K�f�[�^��OPM�ɃZ�b�g����
	void SendTl(int);			//�`�����l�����ʂ��Z�b�g����
	void SendPan(int,int);		//Pan���Z�b�g����
	void SendLfo(int);


public:
	OPMDRV(class Opm *);				//�R���X�g���N�^

	class CHDATA VoTbl[VOMAX];	//���F�e�[�u��
															//SLOT 1�����̐ݒ�N���X
  //MidiProgramName MidiProg[16];
	void setDelta(unsigned int);						//�R�}���hDelta���Ԑݒ�
	void NoteOn(int Ch,int Note,int Vol);		//�m�[�g�n�m
	void NoteOff(int Ch,int Note);					//�m�[�g�n�e�e
	void NoteAllOff(int Ch);								//�S�m�[�g�n�e�e
	void ForceAllOff(int Ch);								//����������~
	void ForceAllOff();											//����������~(�S�`�����l���Ή�)
	void BendCng(int Ch,int Bend);					//�s�b�`�x���h�ω�
	void VolCng(int Ch,int Vol);						//�{�����[���ω�
	void VoCng(int Ch,int VoNum);						//���F�`�F���W
	void MsVolCng(int MVol);								//�}�X�^�[�{�����[���ω�
	void PanCng(int Ch,int Pan);						//Pan�ω�
	int Load(char *);												//���F�ALFO�ݒ胍�[�h
	int Save(char *);												//���F�ALFO�ݒ�Z�[�u
	int getProgNum(void);										//���F�i���o�[�擾
	void setProgNum(long );									//���F�i���o�[�Z�b�g
	void setProgName(char* );								//���F���Z�b�g
	char* getProgName(void);								//���F���擾
	char* getProgName(int);									//���F���擾
	unsigned char getPrm(int);							//index(Vst kNum)�ɂ��p�����[�^�Z�b�g
	void setPrm(int,unsigned char);					//index(Vst kNum)�ɂ��p�����[�^�擾
	void setRPNH(int,int);
	void setNRPNH(int,int);
	void setRPNL(int,int);
	void setNRPNL(int,int);
	void setData(int,int);
	void setPoly(bool);
	void AutoSeq(int sampleframes);
	void setPortaSpeed(int MidiCh,int speed);
	void setPortaOnOff(int MidiCh,bool onoff);
	void setPortaCtr(int MidiCh,int Note);
	void setLFOdelay(int MidiCh,int data);

	~OPMDRV(void);									//�f�X�g���N�^
  //long getMidiProgram(long channel,MidiProgramName *curProg);
	void ChDelay(int );


	void setCcTL_L(int MidiCh,int data,int OpNum);
	void setCcTL_H(int MidiCh,int data,int OpNum);

	void setCcLFQ_L(int MidiCh,int data);
	void setCcLFQ_H(int MidiCh,int data);

	void setCcPMD_L(int MidiCh,int data);
	void setCcPMD_H(int MidiCh,int data);

	void setCcAMD_L(int MidiCh,int data);
	void setCcAMD_H(int MidiCh,int data);

private:
	int CalcLim8(int VoData,int DifData);
	int CalcLim7(int VoData,int DifData);
	int CalcLimTL(int VoData,int DifData);

	void setAMD(int ch,int Amd);
	void setPMD(int ch,int Pmd);
	void setSPD(int ch,int Spd);
	void setTL(int ch,int Tl,int OpNum);
};
#define __OPMDRV__
#endif
