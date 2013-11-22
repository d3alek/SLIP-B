package inf.slip.b.meet4t.bump;

import inf.slip.b.meet4t.R;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.Scanner;

import android.accounts.AccountManager;
import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.api.client.extensions.android.http.AndroidHttp;
import com.google.api.client.googleapis.extensions.android.gms.auth.GoogleAccountCredential;
import com.google.api.client.googleapis.extensions.android.gms.auth.UserRecoverableAuthIOException;
import com.google.api.client.http.FileContent;
import com.google.api.client.http.GenericUrl;
import com.google.api.client.http.HttpResponse;
import com.google.api.client.http.InputStreamContent;
import com.google.api.client.json.gson.GsonFactory;
import com.google.api.services.drive.Drive;
import com.google.api.services.drive.DriveScopes;
import com.google.api.services.drive.model.File;
import com.google.api.services.drive.model.FileList;
import com.google.api.services.drive.model.ParentReference;
import com.google.api.services.drive.model.Permission;

public class BumpFileActivity extends Activity {

	private String mBumpFileType;
	private Uri mBumpFileUri;
	private TextView mInfoText;
	private Drive mService;
	private GoogleAccountCredential mCredential;
	private SharedPreferences mSharedPrefs;
	private ImageView fileImage;
	private TextView fileTitle;


	static final int REQUEST_ACCOUNT_PICKER = 1;
	static final int REQUEST_AUTHORIZATION = 2;
	private static final String BUMP_PREFS_NAME = "BUMP_PREFS";
	protected static final String KEY_FOLDER_ID = "folder_id";
	protected static final String KEY_FILE_ID = "file_id";
	protected static final String TAG = "BumpFileActivity";

	protected void onCreate (Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		// Get intent, action and MIME type
		Intent intent = getIntent();
		String action = intent.getAction();
		String type = intent.getType();

		mBumpFileType = intent.getType();

		mSharedPrefs = getSharedPreferences(BUMP_PREFS_NAME, MODE_PRIVATE);

		Uri uri = intent.getParcelableExtra(Intent.EXTRA_STREAM);
		mBumpFileUri = uri;

		setContentView(R.layout.bump_activity);

		mInfoText = (TextView)findViewById(R.id.info);
		fileImage = (ImageView)findViewById(R.id.fileImage);
		fileTitle = (TextView)findViewById(R.id.fileTitle);

		mInfoText.setText(mBumpFileType + " " + mBumpFileUri);

		//		if (mBumpFileType != null) {
		mCredential = GoogleAccountCredential.usingOAuth2(this, Arrays.asList(DriveScopes.DRIVE)); 
		startActivityForResult(mCredential.newChooseAccountIntent(), REQUEST_ACCOUNT_PICKER);
		//		}
		//		else {

		//		}
	}

	class LoadFilePreviewTask extends AsyncTask<String, String, Bitmap> {

		@Override
		protected Bitmap doInBackground(String... params) {
			String fileId = params[0];
			File file;
			try {
				file = mService.files().get(fileId).execute();
				//				onProgressUpdate(file.getTitle());
				publishProgress(file.getTitle());
				Bitmap bm = downloadImageFileContent(mService, file);
				return bm;
			} 
			catch (IOException e) {
				Toast.makeText(BumpFileActivity.this, "LoadFilePreviewTask failed", Toast.LENGTH_SHORT).show();;
				e.printStackTrace();
			}
			//			fileImage.setIm
			//			fileTitle

			return null;
		}


		@Override
		protected void onProgressUpdate(String... values) {
			fileTitle.setText(values[0]);
			super.onProgressUpdate(values);
		}



		@Override
		protected void onPostExecute(Bitmap result) {
			fileImage.setImageBitmap(result);
			super.onPostExecute(result);
		}

	}

	@Override
	protected void onActivityResult(final int requestCode, final int resultCode, final Intent data) {
		switch (requestCode) {
		case REQUEST_ACCOUNT_PICKER:
			if (resultCode == RESULT_OK && data != null && data.getExtras() != null) {
				String accountName = data.getStringExtra(AccountManager.KEY_ACCOUNT_NAME);
				if (accountName != null) {
					mCredential.setSelectedAccountName(accountName);
				}
			}
			mService = new Drive.Builder(AndroidHttp.newCompatibleTransport(), new GsonFactory(), mCredential).setApplicationName(getString(R.string.app_name)).build();
			if (mBumpFileType == null) {
				String fileId = mSharedPrefs.getString(KEY_FILE_ID, null);
				if (fileId == null) {
					Toast.makeText(this, "No file to share", Toast.LENGTH_SHORT).show();
				}
				else {
					new LoadFilePreviewTask().execute(fileId);
				}
			}
			else {
				saveFileToDrive();
			}
			break;
		case REQUEST_AUTHORIZATION:
			if (resultCode == Activity.RESULT_OK) {
				saveFileToDrive();
			} else {
				startActivityForResult(mCredential.newChooseAccountIntent(), REQUEST_ACCOUNT_PICKER);
			}
			break;
		}
	}

	private void saveFileToDrive() {
		Thread t = new Thread(new Runnable() {
			@Override
			public void run() {
				try {
					// File's binary content
					Log.i(TAG, mBumpFileUri + " " + mBumpFileType);
					//					java.io.File fileContent = new java.io.File(getRealPathFromURI(BumpFileActivity.this, mBumpFileUri));
					InputStream is = getContentResolver().openInputStream(mBumpFileUri);
					//					java.io.File fileContent = new java.io.File(getRealPathFromURI(BumpFileActivity.this, mBumpFileUri));
					//					FileContent mediaContent = new FileContent(mBumpFileType, fileContent);
					InputStreamContent isContent = new InputStreamContent(mBumpFileType, is);

					String folderId = mSharedPrefs.getString(KEY_FOLDER_ID, null);

					if (folderId == null) {
						File folder = null;
						FileList list = mService.files().list().execute();
						for (File file:list.getItems()) {
							if (file.getTitle().equals(getResources().getString(R.string.app_name))) {
								folder = file;
							}
						}
						if (folder != null) {
							File folderDesc = new File();
							folderDesc.setTitle(getResources().getString(R.string.app_name));
							folderDesc.setMimeType("application/vnd.google-apps.folder");
							folder = mService.files().insert(folderDesc).execute();
						}
						Editor e = mSharedPrefs.edit();
						folderId = folder.getId();
						e.putString(KEY_FOLDER_ID, folderId);
						e.apply();
					}

					String title = mBumpFileUri.getLastPathSegment();
					// File's metadata.
					File body = new File();
					body.setTitle(title);
					body.setMimeType(mBumpFileType);

					ParentReference reference = new ParentReference();
					reference.setId(folderId);
					body.setParents(Arrays.asList(reference));
					Permission p = new Permission();
					p.setRole("reader");
					p.setType("anoyone");
					body.setUserPermission(p);
					File file = mService.files().insert(body, isContent).execute();
					if (file != null) {
						showToast("File uploaded: " + file.getTitle());
						Editor e = mSharedPrefs.edit();
						e.putString(KEY_FILE_ID, file.getId());
						e.apply();
					}
				} catch (UserRecoverableAuthIOException e) {
					startActivityForResult(e.getIntent(), REQUEST_AUTHORIZATION);
				} catch (IOException e) {
					showToast("Error uploading file");
					e.printStackTrace();
				}
			}

		});
		t.start();
	}

	public void showToast(final String toast) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				Toast.makeText(getApplicationContext(), toast, Toast.LENGTH_SHORT).show();
			}
		});
	}

	//	public String getRealPathFromURI(Context context, Uri contentUri) {
	//		Cursor cursor = null;
	//		try { 
	//			String[] proj = { MediaStore.Images.Media.DATA };
	//			cursor = context.getContentResolver().query(contentUri,  proj, null, null, null);
	//			int column_index = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
	//			cursor.moveToFirst();
	//			return cursor.getString(column_index);
	//		} finally {
	//			if (cursor != null) {
	//				cursor.close();
	//			}
	//		}
	//	}
	/**
	 * Download the content of the given file.
	 *
	 * @param service Drive service to use for downloading.
	 * @param file File metadata object whose content to download.
	 * @return String representation of file content.  String is returned here
	 *         because this app is setup for text/plain files.
	 * @throws IOException Thrown if the request fails for whatever reason.
	 */
	private String downloadTextFileContent(Drive service, File file)
			throws IOException {
		GenericUrl url = new GenericUrl(file.getDownloadUrl());
		HttpResponse response = service.getRequestFactory().buildGetRequest(url)
				.execute();
		try {
			return new Scanner(response.getContent()).useDelimiter("\\A").next();
		} catch (java.util.NoSuchElementException e) {
			return "";
		}
	}
	private Bitmap downloadImageFileContent(Drive service, File file)
			throws IOException {
		GenericUrl url = new GenericUrl(file.getDownloadUrl());
		HttpResponse response = service.getRequestFactory().buildGetRequest(url)
				.execute();
		try {
			Bitmap b = BitmapFactory.decodeStream(response.getContent());
			int nh = (int) ( b.getHeight() * (512.0 / b.getWidth()) );
			Bitmap scaled = Bitmap.createScaledBitmap(b, 512, nh, true);
			return scaled;
		} catch (java.util.NoSuchElementException e) {
			return null;
		}
	}
}
