using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class palm : MonoBehaviour {
	private float bend;
	private float twist;

	// Use this for initialization
	void Start () {
		
	}

	// Update is called once per frame
	void Update () {

//		float bend = -60f * Input.GetAxis("J") * Time.deltaTime * 88f;
//		float twist = -60f * Input.GetAxis("K") * Time.deltaTime * 88f;

//		float twist = -45f;
//		float bend = -30f;
//
//		float bcos = Mathf.Cos ((bend * Mathf.PI) / 180);
//		float bsin = Mathf.Sin ((bend * Mathf.PI) / 180);
		float tcos = Mathf.Cos ((twist * Mathf.PI) / 180);
		float tsin = Mathf.Sin ((twist * Mathf.PI) / 180);

		// INITIAL ANGLE = (0, 0, 0)

		// to raise hand 45 deg
		// (0, 0, 0) -> (-45, 0, 0)

		// to twist wrist 45 deg
		// (0, 0, 0) -> (0, 0, -45)

		// twist 90, lift 0
		// (0, 0, 0) -> (0, 0, -90)

		// (0, 0, -90)



		// twist 45, lift 45
		// (0, 0, 0) -> (-45, 45, -45)

		// twist 90, lift 90
		// (0, 0, 0) -> (0, 0, -90)

		// x should be bend when twist is 0
		// y should be bend when twist is 0
//		transform.rotation = Quaternion.Euler(new Vector3(bend, 0 - 90, twist));
		// x should be 0 when twist is 90
		// y should be -bend when twist is 90
//		transform.rotation = Quaternion.Euler(new Vector3(0, -bend - 90, twist));

		transform.rotation = Quaternion.Euler(new Vector3(tcos * bend, bend * tsin - 90, twist));

//		transform.rotation = Quaternion.Euler(new Vector3(0, bend - 90, twist));


//		transform.rotation = Quaternion.Euler(new Vector3(-55, -90, 0));




//		  0, 0
//		transform.rotation = Quaternion.Euler(new Vector3(0, -90, 0));
//		-30, 0
//		transform.rotation = Quaternion.Euler(new Vector3(0, -90, -30));
//		-45, 0
//		transform.rotation = Quaternion.Euler(new Vector3(0, -90, -45));
//		-60, 0
//		transform.rotation = Quaternion.Euler(new Vector3(0, -90, -60));
//		-90, 0
//		transform.rotation = Quaternion.Euler(new Vector3(0, -90, -90));


//		  0, -90
//		transform.rotation = Quaternion.Euler(new Vector3(-90, 0, -90));
//		-30, -90
//		transform.rotation = Quaternion.Euler(new Vector3(-60, 0, -90));
//		-45, -90
//		transform.rotation = Quaternion.Euler(new Vector3(-45, 0, -90));
//		-60, -90
//		transform.rotation = Quaternion.Euler(new Vector3(-30, 0, -90));
//		-90, -90
//		transform.rotation = Quaternion.Euler(new Vector3(0, 0, -90));
	}

	public void BendHand (float amount) {
		bend = amount;
	}

	public void TwistHand (float amount) {
		twist = amount;
	}
}
