/****************************************************************************

    OpPoint Class
	Copyright (C) 2003 Andre Deperrois adeperrois@xflr5.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/


// OpPoint.cpp: implementation of the OpPoint class.
// The Class associated to OpPoint objects
// resulting from XFoil calculations


#include "OpPoint.h"
#include "../Globals.h"
#include "../MainFrame.h"
#include "../Serializer.h"


OpPoint* OpPoint::newBySerialize() {
    OpPoint* oppoint = new OpPoint ();
    oppoint->serialize();
    return oppoint;
}

OpPoint::OpPoint(QString name, StorableObject *parent)
    : StorableObject (name, parent)
{
    m_bVisc     = false;//not a  viscous point a priori
	m_bDispSurf = false;// no boundary layer surface either
	m_bTEFlap   = false;
	m_bLEFlap   = false;

	Xtr1   = 0.0;
	Xtr2   = 0.0;
	XForce = 0.0;
	YForce = 0.0;
	Cpmn   = 0.0;
	m_XCP  = 0.0;
	m_LEHMom   = 0.0; m_TEHMom = 0.0;

	memset(Qi,  0, sizeof(Qi));
	memset(Qv,  0, sizeof(Qv));
	memset(Cpi,  0, sizeof(Cpi));
	memset(Cpv,  0, sizeof(Cpv));
	memset(x,  0, sizeof(y));
	memset(y,  0, sizeof(x));

    memset(iblte,  0, sizeof(iblte));
    memset(ipan,  0, sizeof(ipan));
    memset(nbl,  0, sizeof(nbl));
    memset(dstr,  0, sizeof(dstr));

	memset(xd1,  0, sizeof(xd1));
	memset(xd2,  0, sizeof(xd2));
	memset(xd3,  0, sizeof(xd3));
	memset(yd1,  0, sizeof(yd1));
	memset(yd2,  0, sizeof(yd2));
	memset(yd3,  0, sizeof(yd3));
	nd1 = 0;
	nd2 = 0;
	nd3 = 0;
//	Format = 1;

	m_bIsVisible  = true;
	m_bShowPoints = false;
	m_Style = 0;
	m_Width = 1;
    m_Color = QColor(255,0,100,127);

}

//void OpPoint::serialize() {
//    StorableObject::serialize();

//}



// This Object is  used to store the data issued from an XFoil Calculation
// an array of OperatingPoints is a CPolar


bool OpPoint::SerializeOp(QDataStream &ar, bool bIsStoring, int ArchiveFormat)
{
	int a, b, k, Format;
	float f,gg;

    if(bIsStoring)
    {
		ar << 100003;
		//100003 : suppressed archiving of s coordinate
		//100002 : first numbered archiveformat
		//write variables
		WriteCString(ar, m_strFoilName);
		WriteCString(ar, m_strPlrName);
		ar << (float)Reynolds << (float)Mach << (float)Alpha;
		ar << n << nd1 << nd2 << nd3;
		if(m_bVisc)     a=1; else a=0;
		if(m_bDispSurf) b=1; else b=0;

		ar << a << b;
		ar << (float)Cl << (float)Cm << (float)Cd << (float)Cdp;
		ar << (float)Xtr1 << (float)Xtr2 << (float)ACrit << (float)m_TEHMom << (float)Cpmn;
		for (k=0; k<n; k++)	ar << (float)Cpv[k] << (float)Cpi[k];
//		for (k=0; k<n; k++)	    ar << (float)s[k] << (float)Qv[k] << (float)Qi[k];
		for (k=0; k<n; k++)	    ar << (float)Qv[k] << (float)Qi[k];
		for (k=0; k<=nd1; k++)  ar << (float)xd1[k] << (float)yd1[k];
		for (k=0; k<nd2; k++)	ar << (float)xd2[k] << (float)yd2[k];
		for (k=0; k<nd3; k++)	ar << (float)xd3[k] << (float)yd3[k];
		ar<<m_Style<<m_Width;
		WriteCOLORREF(ar,m_Color);
		if(m_bIsVisible)     ar<<1; else ar<<0;
		if(m_bShowPoints)    ar<<1; else ar<<0;
	}
	else
	{

        if(ArchiveFormat>=100002) ar>>Format;
        else Format = 0;
        //read variables
		ReadCString(ar, m_strFoilName);
		ReadCString(ar, m_strPlrName);

        ar >> f; Reynolds =f;
        ar >> f; Mach = f;
        ar >> f; Alpha = f;
        ar >> n >> nd1 >> nd2 >> nd3;
        ar >> a >> b;
        if(a) m_bVisc = true; else m_bVisc = false;
        if(a!=0 && a!=1){
            m_strFoilName = "";
            return false;
        }
        if(b) m_bDispSurf = true; else m_bDispSurf = false;
        if(b!=0 && b!=1){
            m_strFoilName = "";
            return false;
        }

        ar >> f; Cl = f;
        ar >> f; Cm = f;
        ar >> f; Cd = f;
        ar >> f; Cdp = f;
        ar >> f; Xtr1 = f;
        ar >> f; Xtr2 = f;
        ar >> f; ACrit =f;
        ar >> f; m_TEHMom = f;
        ar >> f; Cpmn = f;
        for (k=0; k<n; k++)	{
            ar >> f; Cpv[k] = f;
            ar >> f; Cpi[k] = f;
        }

//			if (Format ==2) {
		for (k=0; k<n; k++)
		{
			if(Format<=100002)	ar >> f; //s[k]  = f;
			ar >> f; Qv[k] = f;
			ar >> f; Qi[k] = f;
		}
//			}
		for (k=0; k<=nd1; k++)
		{
			ar >> f >> gg;
            xd1[k] = f;
			yd1[k] = gg;
        }
		for (k=0; k<nd2; k++)
		{
			ar >> f >> gg;
            xd2[k] = f;
			yd2[k] = gg;
        }
		for (k=0; k<nd3; k++)
		{
			ar >> f >> gg;
            xd3[k] = f;
			yd3[k] = gg;
        }
		if(ArchiveFormat>=100002)
		{
			ar>>m_Style>>m_Width;
			ReadCOLORREF(ar, m_Color);

			ar >> a ;
			if(a!=0 && a!=1)
			{
                m_strFoilName = "";
                return false;
            }
            if(a) m_bIsVisible = true; else m_bIsVisible = false;

            ar >> a ;
            if(a!=0 && a!=1){
                m_strFoilName = "";
                return false;
            }
            if(a) m_bShowPoints = true; else m_bShowPoints = false;
        }
	}
	return true;
}


bool OpPoint::ExportOpp(QTextStream &out, QString Version, int FileType, bool bDataOnly)
{
	int k;
	QString strong;

	if(!bDataOnly)
	{
		out << Version+"\n";

		strong = m_strFoilName + "\n";
		out<< strong;
		strong = m_strPlrName + "\n";
		out<< strong;
		if(FileType==1) strong=QString("Alpha = %1,  Re = %2,  Ma = %3,  ACrit =%4 \n\n")
									   .arg(Alpha,5,'f',1).arg(Reynolds,8,'f',0).arg(Mach,6,'f',4).arg(ACrit,4,'f',1);
		else            strong=QString("Alpha =, %1,  Re =, %2,  Ma =, %3,  ACrit =, %4 \n\n")
									   .arg(Alpha,5,'f',1).arg(Reynolds,8,'f',0).arg(Mach,6,'f',4).arg(ACrit,4,'f',1);
		out<< strong;
	}

	if(FileType==1) out << "   x        Cpi      Cpv        Qi        Qv\n";
	else            out << "x,Cpi,Cpv,Qi,Qv\n";

	for (k=0; k<n; k++)
	{
		if(FileType==1) strong=QString("%1  %2   %3   %4   %5\n")
									   .arg(x[k],7,'f',4).arg(Cpi[k],7,'f',3).arg(Cpv[k],7,'f',3).arg(Qi[k],7,'f',3).arg(Qv[k],7,'f',3);
		else            strong=QString("%1,%2,%3,%4,%5\n")
									   .arg(x[k],7,'f',4).arg(Cpi[k],7,'f',3).arg(Cpv[k],7,'f',3).arg(Qi[k],7,'f',3).arg(Qv[k],7,'f',3);
		out<< strong;
	}

	out << "\n\n";

	return true;
}


void OpPoint::GetOppProperties(QString &OpPointProperties, bool bData)
{
	QString strong;
	OpPointProperties.clear();

	strong  = QString(QObject::tr("Re")+"    = %1 ").arg(Reynolds,7,'f',0);
	OpPointProperties += strong +"\n";

	strong  = QString(QObject::tr("Alpha")+" = %1").arg(Alpha,6,'f',2);
	OpPointProperties += strong +QString::fromUtf8("deg")+"\n";

	strong  = QString(QObject::tr("Mach")+"  = %1 ").arg(Mach,7,'f',3);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("NCrit")+" = %1 ").arg(ACrit,5,'f',1);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("CL")+"    = %1 ").arg(Cl,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("CD")+"    = %1 ").arg(Cd,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Cm")+"    = %1 ").arg(Cm,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Cdp")+"   = %1 ").arg(Cdp,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Cpmn")+"  = %1 ").arg(Cpmn,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("XCP")+"   = %1 ").arg(m_XCP,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Top Transition")+" = %1 ").arg(Xtr1,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Bot Transition")+" = %1 ").arg(Xtr2,9,'f',5);
	OpPointProperties += strong + "\n";

	if(m_bTEFlap)
	{
		strong  = QString(QObject::tr("T.E. Flap moment")+" = %1 ").arg(m_TEHMom,9,'f',5);
		OpPointProperties += strong + "\n";
	}
	if(m_bLEFlap)
	{
		strong  = QString(QObject::tr("L.E. Flap moment")+" = %1 ").arg(m_LEHMom,9,'f',5);
		OpPointProperties += strong + "\n";
	}

	if(!bData) return;
	QTextStream out;
	strong.clear();
	out.setString(&strong);
    ExportOpp(out, g_mainFrame->m_VersionName, g_mainFrame->m_ExportFileType, true);
	OpPointProperties += "\n"+strong;
}

void OpPoint::serialize() {
    StorableObject::serialize();

    g_serializer.readOrWriteBool (&m_bVisc);
    g_serializer.readOrWriteBool (&m_bDispSurf);
    g_serializer.readOrWriteBool (&m_bTEFlap);
    g_serializer.readOrWriteBool (&m_bLEFlap);

    g_serializer.readOrWriteInt (&n);
    g_serializer.readOrWriteInt (&nd1);
    g_serializer.readOrWriteInt (&nd2);
    g_serializer.readOrWriteInt (&nd3);
    g_serializer.readOrWriteInt (&m_Style);
    g_serializer.readOrWriteInt (&m_Width);

    g_serializer.readOrWriteDouble (&Reynolds);
    g_serializer.readOrWriteDouble (&Mach);
    g_serializer.readOrWriteDouble (&Alpha);
    g_serializer.readOrWriteDouble (&Cl);
    g_serializer.readOrWriteDouble (&Cm);
    g_serializer.readOrWriteDouble (&Cd);
    g_serializer.readOrWriteDouble (&Cdp);
    g_serializer.readOrWriteDouble (&Xtr1);
    g_serializer.readOrWriteDouble (&Xtr2);
    g_serializer.readOrWriteDouble (&ACrit);
    g_serializer.readOrWriteDouble (&m_XCP);
    g_serializer.readOrWriteDouble (&m_TEHMom);
    g_serializer.readOrWriteDouble (&m_LEHMom);
    g_serializer.readOrWriteDouble (&XForce);
    g_serializer.readOrWriteDouble (&YForce);
    g_serializer.readOrWriteDouble (&Cpmn);

    g_serializer.readOrWriteDoubleArray1D (x, IQX);
    g_serializer.readOrWriteDoubleArray1D (y, IQX);
    g_serializer.readOrWriteDoubleArray1D (Cpv, IQX);
    g_serializer.readOrWriteDoubleArray1D (Cpi, IQX);
    g_serializer.readOrWriteDoubleArray1D (Qv, IQX);
    g_serializer.readOrWriteDoubleArray1D (Qi, IQX);
    g_serializer.readOrWriteDoubleArray1D (xd1, IQX);
    g_serializer.readOrWriteDoubleArray1D (yd1, IQX);
    g_serializer.readOrWriteDoubleArray1D (xd2, IWX);
    g_serializer.readOrWriteDoubleArray1D (yd2, IWX);
    g_serializer.readOrWriteDoubleArray1D (xd3, IWX);
    g_serializer.readOrWriteDoubleArray1D (yd3, IWX);

    g_serializer.readOrWriteString (&m_strFoilName);
    g_serializer.readOrWriteString (&m_strPlrName);

    g_serializer.readOrWriteColor (&m_Color);
}

void OpPoint::CreateXBL(double xs[IVX][3],int &nside1, int &nside2)
{
//	double xxtr[3];

    int i;
//---- set up cartesian bl x-arrays for plotting
    for(int is=1; is<= 2; is++){
        for (int ibl=2; ibl<= nbl[is]; ibl++){
            i = ipan[ibl][is];
            xs[ibl][is] = x[i];
            qDebug() << "Alpha: " << Alpha << ", Ipan: " << i << ", X: " << x[i] << ", IDX: "<<ibl;

//			xxtr[is] = xle + (xte-xle)*xoctr[is] - (yte-yle)*yoctr[is];
        }
    }

    nside1 = nbl[2] + iblte[1] - iblte[2];
    nside2 = nbl[2];

    for( int iblw=1; iblw <= nbl[2]-iblte[2]; iblw++)
        xs[iblte[1]+iblw][1] = xs[iblte[2]+iblw][2];
}

void OpPoint::FillHk(double ws[IVX][3], int nside1, int nside2)
{
    int nside[3];
    nside[1] = nside1;
    nside[2] = nside2;
    double thi, dsi, uei, uc, amsq, dummy;
    double hstinv = gamm1*(Mach/qinf)*(Mach/qinf) / (1.0 + 0.5*gamm1*Mach*Mach);

    //---- fill kinematic shape parameter array
    for (int is=1; is<=2; is++){
        for(int ibl=2; ibl< nside[is]; ibl++){

            thi = thet[ibl][is];
            dsi = dstr[ibl][is];
            uei = uedg[ibl][is];
            uc = uei * (1.0-tklam) / (1.0 - tklam*(uei/qinf)*(uei/qinf));
            amsq = uc*uc*hstinv / (gamm1*(1.0 - 0.5*uc*uc*hstinv));
            hkin(dsi/thi, amsq, ws[ibl][is], dummy, dummy);
        }
    }
}

void OpPoint::FillRTheta(double ws[IVX][3], int nside1, int nside2)
{
    int nside[3];
    nside[1] = nside1;
    nside[2] = nside2;
    double ue, herat, rhoe, amue, uei;
//---- 1 / (total enthalpy)
    double hstinv = gamm1*(Mach/qinf)*(Mach/qinf) / (1.0 + 0.5*gamm1*Mach*Mach);

//---- Sutherland's const./to   (assumes stagnation conditions are at stp)
    double hvrat = 0.35;

//---- fill rtheta arrays
    for (int is=1; is<=2; is++){
        for(int ibl=2; ibl< nside[is]; ibl++){
            uei = uedg[ibl][is];
            ue  = uei * (1.0-tklam) / (1.0 - tklam*(uei/qinf)*(uei/qinf));
            herat = (1.0 - 0.5*hstinv*ue  *ue)
                / (1.0 - 0.5*hstinv*qinf*qinf);
            rhoe = pow(herat, 1.0/gamm1);
            amue = sqrt(herat*herat*herat) * (1.0+hvrat)/(herat+hvrat);
            ws[ibl][is] = Reynolds * rhoe*ue*thet[ibl][is]/amue;
        }
    }
}

bool OpPoint::hkin(double h, double msq, double &hk, double &hk_h, double &hk_msq){
    //---- calculate kinematic shape parameter (assuming air)
    //     (from Whitfield )
    hk     =    (h - 0.29*msq)   /(1.0 + 0.113*msq);
    hk_h   =     1.0              /(1.0 + 0.113*msq);
    hk_msq = (-.29 - 0.113*(hk))/(1.0 + 0.113*msq);

    return true;
}
bool OpPoint::getLvconv() const
{
    return m_Lvconv;
}

void OpPoint::setLvconv(bool Lvconv)
{
    m_Lvconv = Lvconv;
}



double OpPoint::getReynolds() const
{
    return Reynolds;
}

double OpPoint::getMach() const
{
    return Mach;
}

double OpPoint::getAlpha() const
{
    return Alpha;
}

int OpPoint::getNblAt(int _x)
{
    return nbl[_x];
}

int OpPoint::getIblteAt(int _x)
{
    return iblte[_x];
}

int OpPoint::getIpanAt(int _x, int _y)
{
    return ipan[_x][_y];
}

double OpPoint::getXAt(int _x)
{
    return x[_x];
}

void OpPoint::setReynolds(double value)
{
    Reynolds = value;
}

void OpPoint::setAlpha(double value)
{
    Alpha = value;
}

double OpPoint::getDstrAt(int _x, int _y)
{
    return dstr[_x][_y];
}



