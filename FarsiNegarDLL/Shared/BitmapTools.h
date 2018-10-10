#pragma once
#include "m.h"

namespace Mad{

//Bitmap Class
class BitmapTools
{
	// create rect bitmap filled with rgbColor
	Public static HBITMAP CreateRect(CSize size, COLORREF rgbColor, COLORREF rgbBorderColor, UINT bitCount)
	{
		HDC memDC = CreateCompatibleDC(NULL);
		TESTLASTERR(memDC!=NULL);

		HBITMAP hBmp = CreateDIBSection(size, bitCount);
		
		TESTLASTERR(hBmp!=NULL);

		HBITMAP hOldBmp = (HBITMAP)SelectObject(memDC, hBmp);
		TESTLASTERR(hOldBmp!=NULL);

		CRect rc(0, 0, size.cx, size.cy);
		m::DC_FillSolidRect(memDC, rc, rgbBorderColor);
		rc.DeflateRect(1,1,1,1);
		m::DC_FillSolidRect(memDC, rc, rgbColor);
		
		HBITMAP ret = Create(memDC, CRect(0, 0, size.cx, size.cy), bitCount);
		
		//clean up
		TESTLASTERR( SelectObject(memDC, hOldBmp)!=NULL );
		TESTLASTERR( DeleteDC(memDC) );
		TESTLASTERR( DeleteObject(hBmp) );
		return ret;
	}

	Public static HBITMAP CreateRect(CSize size, COLORREF rgbColor, UINT bitCount)
	{
		return CreateRect(size, rgbColor, rgbColor, bitCount);
	}

	Public static HBITMAP CreateDIBSection(CSize size, UINT bitCount)
	{
		// create our DIB section and select the bitmap into the dc
		BITMAPINFO bmi;        // bitmap header
		VOID *pvBits;          // pointer to DIB section
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = size.cx;
		bmi.bmiHeader.biHeight = size.cy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = (WORD)bitCount;         // four 8-bit components
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * bmi.bmiHeader.biBitCount/8;
		HBITMAP bitmapHandle = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
		return bitmapHandle;
	}

	//for gray scale bitmap set, fStyle=ILD_TRANSPARENT; fState=ILS_SATURATE 
	//@param bitmapHandle must be 32bit alpha bitmap
	//@return 32 bit alpha bitmap
	//@see ImageList_DrawIndirect
	Public static HBITMAP GetEffected(HBITMAP bitmapHandle, UINT fStyle, DWORD fState=ILS_NORMAL, DWORD Frame = 0)
	{
		CSize bitmapSize = GetSize(bitmapHandle);

		//create temporary imagelist
		HIMAGELIST imagelistHandle = ImageList_Create(bitmapSize.cx, bitmapSize.cy, ILC_COLOR32, 0, 1);
		int imageIndex = ImageList_Add(imagelistHandle, bitmapHandle, 0);
		
		//create compatible dc
		HDC dcHandle = CreateCompatibleDC(NULL);
		TESTLASTERR(dcHandle!=NULL);

	    // create our DIB section and select the bitmap into the dc
		HBITMAP newBitmap = CreateDIBSection(bitmapSize, 32);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(dcHandle, newBitmap);

		IMAGELISTDRAWPARAMS ildp = { 0 };
		ildp.cbSize = sizeof(IMAGELISTDRAWPARAMS);
		ildp.himl = imagelistHandle;
		ildp.i = imageIndex;
		ildp.hdcDst = dcHandle;
		ildp.x = 0;
		ildp.y = 0;
		ildp.cx = 0;
		ildp.cy = 0;
		ildp.xBitmap = 0;
		ildp.yBitmap = 0;
		ildp.rgbBk = CLR_NONE;
		ildp.fStyle = fStyle;
		fState; //perevent level 4 warnings
		Frame; //perevent level 4 warnings
		__if_exists(IMAGELISTDRAWPARAMS::fState){
			ildp.fState = fState;
			ildp.Frame = Frame;
		}
		ATLENSURE( ImageList_DrawIndirect(&ildp) );

		SelectObject(dcHandle, oldBitmap);
		DeleteDC(dcHandle);
		return newBitmap;
	}

	Public static HBITMAP Clone(HBITMAP hSource)
	{
		return (HBITMAP)CopyImage(hSource, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	}

	Public static HBITMAP OleLoadPictureFile(Path fileName)
	{
		// open file
		HANDLE hFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if ( INVALID_HANDLE_VALUE == hFile)
			return false;

		// get file size
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		ATLASSERT(-1 != dwFileSize);

		LPVOID pvData = NULL;
		// alloc memory based on file size
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
		ATLASSERT(NULL != hGlobal);

		pvData = GlobalLock(hGlobal);
		ATLASSERT(NULL != pvData);

		DWORD dwBytesRead = 0;
		// read file and store in global memory
		BOOL bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
		GlobalUnlock(hGlobal);
		CloseHandle(hFile);
		if ( !bRead ) 
		{
			GlobalFree(hGlobal);
			return NULL;
		}


		LPSTREAM pstm = NULL;
		// create IStreamPtr from global memory
		HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);
		ATLASSERT(SUCCEEDED(hr) && pstm);

		// Create IPicture from image file
		IPicturePtr ppicture;
		hr = OleLoadPicture(pstm, dwFileSize, FALSE, IID_IPicture, (LPVOID *)&ppicture);
		if ( FAILED(hr) ) 
		{
			GlobalFree(hGlobal);
			return NULL;
		}

		
		long hmWidth;
		long hmHeight;
		ppicture->get_Width(&hmWidth);
		ppicture->get_Height(&hmHeight);
		
		// convert himetric to pixels
		HWND hWnd = GetDesktopWindow();
		HDC hdc = GetDC(hWnd);
		int width	= MulDiv(hmWidth, GetDeviceCaps(hdc, LOGPIXELSX), HIMETRIC_INCH);
		int height	= MulDiv(hmHeight, GetDeviceCaps(hdc, LOGPIXELSY), HIMETRIC_INCH);
		ReleaseDC(hWnd, hdc);
		
		CRect rcBmp(0, 0, width, height);
		OLE_HANDLE olehBmp;
		ppicture->get_Handle(&olehBmp);

		HBITMAP bitmapHandle;
		memcpy(&bitmapHandle, &olehBmp, sizeof HBITMAP);
		HBITMAP hRet = Create(bitmapHandle, rcBmp, 0);
		
		// display picture using IPicture::Render
		//Render(hdc, 0, 0, width, height, 0, hmHeight, hmWidth, -hmHeight, &rc);

		GlobalFree(hGlobal);
		return hRet;
	}

	// create new bitmap with new size from hSrcBmp; if new size larger than hSrcBmp , new bitmap will places _
	//  in center and aditional area filled with rgbFillColor;
	// @param rgbFillColor the color of additional area if new size larger  than hSrcBmp
	// @param stretch if true and new size of bitmap (with or height) smaller than hSrcBmp's size, it will be streched to new size
	// @param bDeleteSource if true and new bitmap created, hSrcBmp will deleted
	// @return NULL if failed
	Public static HBITMAP ChangeSize(HBITMAP hSrcBmp, CSize newSize, COLORREF rgbFillColor, bool stretch=false)
	{
		ImagePtr image = new Mad::Image(hSrcBmp);
		CSize srcSize(image->GetWidth(), image->GetHeight());

		Gdiplus::Bitmap* bitmapSrc;
		if (image->GetBPP()==32)
		{
			Gdiplus::Bitmap bmp( image->GetWidth(), image->GetHeight(), image->GetPitch(), PixelFormat32bppARGB, (BYTE*)image->GetBits() );
			bitmapSrc = bmp.Clone(0, 0, image->GetWidth(), image->GetHeight(), PixelFormat32bppARGB);
		}
		else
		{
			bitmapSrc = Gdiplus::Bitmap::FromHBITMAP(hSrcBmp, NULL);
		}
		image->Detach();
		

		//calcualte new position
		CPoint ptStart = CPoint(0,0);
		Gdiplus::Rect desRect(0, 0, newSize.cx, newSize.cy);
		stretch = stretch || srcSize.cx>newSize.cx || srcSize.cy>newSize.cy;
		if (!stretch)
		{
			desRect.X = (srcSize.cx>newSize.cx) ? 0 : (newSize.cx - srcSize.cx)/2;
			desRect.Y = (srcSize.cy>newSize.cy) ? 0 : (newSize.cy - srcSize.cy)/2;
			desRect.Width = srcSize.cx;
			desRect.Height = srcSize.cy;
		}

		Gdiplus::Bitmap bitmapDes( newSize.cx, newSize.cy, bitmapSrc->GetPixelFormat());
		Gdiplus::Graphics* graphics = Gdiplus::Graphics::FromImage(&bitmapDes);
		graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);

		graphics->DrawImage(bitmapSrc, desRect, 0, 0, bitmapSrc->GetWidth(), bitmapSrc->GetHeight(), Gdiplus::UnitPixel);
		delete graphics;
		delete bitmapSrc;

		HBITMAP ret;
		Gdiplus::Color backColor;
		backColor.SetFromCOLORREF(rgbFillColor);
		bitmapDes.GetHBITMAP(backColor, &ret);
		return ret;
	}

	Public static void DrawTransparent(HDC hdc, HBITMAP bitmapHandle, int x, int y, COLORREF transparentColor, int width=-1, int height=-1)
	{
		CSize bitmapSize = GetSize(bitmapHandle);
		if (width==-1) width = bitmapSize.cx;
		if (height==-1) height = bitmapSize.cy;
		
		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP oldMeBitmapTools = (HBITMAP)SelectObject(memDC, bitmapHandle);
		BOOL res = TransparentBlt(hdc, x, y, width, height, memDC, 0, 0, width, height, transparentColor);

		//Clean up
		SelectObject(memDC, oldMeBitmapTools);
		DeleteDC(memDC);
		TESTLASTERR(res);
	}

	Public static void Draw(HDC hdc, HBITMAP bitmapHandle, int x, int y, int width=-1, int height=-1, bool stretch=false, DWORD dwRop=SRCCOPY)
	{
		CSize bmpSize = GetSize(bitmapHandle);
		if (width==-1) width=bmpSize.cx;
		if (height==-1) height=bmpSize.cy;

		HDC memDC = CreateCompatibleDC(NULL);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmapHandle);

		BOOL res;
		if (stretch)
			res = StretchBlt(hdc, x, y, width, height, memDC, 0, 0, bmpSize.cx, bmpSize.cy, dwRop);
		else
			res = BitBlt(hdc, x, y, width, height, memDC, 0, 0, dwRop);

		//Clean up
		SelectObject(memDC, oldBitmap);
		DeleteDC(memDC);
		TESTLASTERR(res);
	}

	// Create (extract) bitmap from another bitmap or hdc
	Public static HBITMAP Create(HBITMAP hSrcBmp, CRect rect, UINT bitCount)
	{
		HDC memDC = CreateCompatibleDC(NULL); //{create
		TESTLASTERR(memDC!=NULL);
		
		HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hSrcBmp);
		TESTLASTERR(oldBitmap!=NULL);
		
		HBITMAP ret = Create(memDC, rect, bitCount);
		TESTLASTERR(ret!=NULL);
		
		//cleanup
		TESTLASTERR( SelectObject(memDC, oldBitmap)!=NULL );
		TESTLASTERR( DeleteDC(memDC) ); //}delete
		return ret;
	}

	//create bitmap from given hdc
	Public static HBITMAP Create(HDC hdc, CRect rect, UINT bitCount)
	{
		HBITMAP hBmp = (bitCount==0)
			? CreateCompatibleBitmap(hdc , rect.Width(), rect.Height())
			: CreateDIBSection(rect.Size(), bitCount);//{create
		TESTLASTERR(hBmp!=NULL);

		HDC memDC = CreateCompatibleDC(hdc); //{create
		TESTLASTERR(memDC!=NULL);

		HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBmp);
		TESTLASTERR(oldBitmap!=NULL);

		BitBlt(memDC, 0, 0, rect.Width(), rect.Height(), hdc, rect.left, rect.top, SRCCOPY);
		
		hBmp = (HBITMAP)SelectObject(memDC, oldBitmap);
		TESTLASTERR(hBmp!=NULL);

		TESTLASTERR(DeleteDC(memDC)); //}delete
		return hBmp;
	}

	Public static void DrawTile(HDC hdcDest, int x, int y, int width, int height, HBITMAP bitmapHandle, int xSrc=0, int ySrc=0,int nWidthSrc=-1, int nHeightSrc=-1, DWORD dwRop=SRCCOPY)
	{
		CSize size = GetSize(bitmapHandle);

		if (nWidthSrc==-1) nWidthSrc= size.cx;
		if (nHeightSrc==-1) nHeightSrc= size.cy;

		HDC memDC;
		memDC = CreateCompatibleDC(hdcDest);
		TESTLASTERR(memDC!=NULL);

		HBITMAP oldMeBitmapTools = (HBITMAP)SelectObject(memDC, bitmapHandle);
		DrawTile(hdcDest, x, y, width, height, memDC, xSrc, ySrc, nWidthSrc, nHeightSrc, dwRop);

		//clean up
		SelectObject(memDC, oldMeBitmapTools);
		TESTLASTERR( DeleteDC(memDC) );
	}
	
	Public static void DrawTile(HDC hdcDest, int x, int y, int width, int height, HDC hdcSrc, int xSrc, int ySrc,int nWidthSrc, int nHeightSrc, DWORD dwRop=SRCCOPY)
	{
		for (int ly=y; ly<y+height; ly+= nHeightSrc)
		{
			for (int lx=x; lx<x+width; lx +=nWidthSrc)
			{
				TESTLASTERR( BitBlt(hdcDest, lx, ly, width , height, hdcSrc, xSrc, ySrc, dwRop) );
			}
		}
	}

	Public static CSize GetSize(HBITMAP bitmapHandle)
	{
		BITMAP bitmap;
		TESTLASTERR(GetObject(bitmapHandle, sizeof bitmap, &bitmap));
		return CSize(bitmap.bmWidth, bitmap.bmHeight);
	}

	Public static COLORREF GetPixel(HBITMAP bitmapHandle, CPoint pt)
	{
		CImage image;
		image.Attach(bitmapHandle);
		COLORREF ret = image.GetPixel(pt.x, pt.y);
		image.Detach();
		return ret;
	}

};

class ImageListTools
{
	// @return NULL if failed; otherwise handle of created bitmap; this handle must deleted when not needed
	Public static HBITMAP ExtractBitmap(HIMAGELIST himl, int nIndex, int bitCount)
	{
		ATLASSERT (himl);

		IMAGEINFO inf;
		ImageList_GetImageInfo(himl, nIndex, &inf);

		if (inf.hbmImage==NULL)
			return NULL;
		
		HBITMAP hBmp = (HBITMAP)CopyImage(inf.hbmImage, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG|LR_CREATEDIBSECTION); //{create
		HBITMAP hBmpRet = BitmapTools::Create(hBmp, &inf.rcImage, bitCount); //{create
		DeleteObject(hBmp); //}delete
		return hBmpRet;
	}


	// Retrieves the dimensions of images in an image list. All images in an image list have the same dimensions. 
	// @see ImageList_GetIconSize in SDK
	Public static CSize GetIconSize(HIMAGELIST imageListHandle)
	{
		int cx,cy;
		ATLENSURE( ::ImageList_GetIconSize(imageListHandle, &cx, &cy) );
		return CSize(cx, cy);
	}

	// same as ImageList_Copy in SDK but it work to all version of windows and himlDst and himlSrc not need to be idendical( in SDK say that both parameters must be identical.)
	// @param uFlags must be one of the ILCF_MOVE or ILCF_SWAP 
	// @see ImageList_Copy in SDK
	Public static BOOL Copy(HIMAGELIST himlDst, int iDst, HIMAGELIST himlSrc, int iSrc, bool bRepalce = false)
	{
		BOOL ret = TRUE;
		HBITMAP hSrcBmp = NULL;
		HBITMAP hSrcBmpMask = NULL;
		HBITMAP hDstBmp = NULL;
		HBITMAP hDstBmpMask = NULL;
		HBITMAP hBmp; //used to convert other bitmaps to exact size
		CSize destImageSize = GetIconSize(himlDst);
		CSize srcImageSize = GetIconSize(himlSrc);

		IMAGEINFO inf;
		if (!ImageList_GetImageInfo(himlSrc, iSrc, &inf))
			return FALSE;
		
		hBmp = (HBITMAP)CopyImage(inf.hbmImage, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG|LR_CREATEDIBSECTION); //{create
		hSrcBmp = BitmapTools::Create(hBmp, &inf.rcImage, 0); //{create
		DeleteObject(hBmp); //}delete
		
		if(inf.hbmMask){
			hBmp = (HBITMAP)CopyImage(inf.hbmMask, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG|LR_CREATEDIBSECTION); //{create
			hSrcBmpMask = BitmapTools::Create(hBmp, &inf.rcImage, 0); //{create
			DeleteObject(hBmp); //}delete
		}

		//enlarge source size if source is smaller than source
		if (destImageSize.cx>srcImageSize.cx || destImageSize.cy>srcImageSize.cy)
		{
			hBmp = hSrcBmp;
			hSrcBmp = BitmapTools::ChangeSize(hBmp, destImageSize, ImageList_GetBkColor(himlSrc), false);
			DeleteObject(hBmp);

			if (hSrcBmpMask)
			{
				hBmp = hSrcBmpMask;
				hSrcBmpMask = BitmapTools::ChangeSize(hBmp, destImageSize, RGB(255,255,255), false);
				DeleteObject(hBmp);
			}
		}
		
		//store destination bitmap if repalce needed
		if (bRepalce)
		{
			ImageList_GetImageInfo(himlDst, iDst, &inf);
			hBmp = (HBITMAP)CopyImage(inf.hbmImage, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG|LR_CREATEDIBSECTION); //{create
			hDstBmp = BitmapTools::Create(hBmp, &inf.rcImage, 0); //{create
			DeleteObject(hBmp); //}delete

			if(inf.hbmMask){
				hBmp = (HBITMAP)CopyImage(inf.hbmMask, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG|LR_CREATEDIBSECTION); //{create
				hDstBmpMask = BitmapTools::Create(hBmp, &inf.rcImage, 0); //{create
				DeleteObject(hBmp); //}delete
			}

			ret = ImageList_Replace(himlSrc, iSrc, hDstBmp, hDstBmpMask);
			if (!ret) goto error;
		}
		
		//replace
		ret = ImageList_Replace(himlDst, iDst, hSrcBmp, hSrcBmpMask);

	error:
		//cleanup
		DeleteObject(hSrcBmp); //}delete
		DeleteObject(hSrcBmpMask); //}delete
		DeleteObject(hDstBmp); //}delete
		DeleteObject(hDstBmpMask); //}delete
		return ret;
	}
};

}//namespace Mad