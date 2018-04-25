using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class finger4Seg3 : MonoBehaviour {

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}

	public void Bend (float first, float amount, Vector3 palm) {
		float nAmount = 0.0f;

		if (amount >= 0.7) {
			nAmount = 1;
		} else if (amount <= 0.5) {
			nAmount = 0;
		} else {
			nAmount = (amount - 0.5f) / 0.2f;
		}

		transform.rotation = Quaternion.Euler(new Vector3(0, 0, palm.x + 90 + first + nAmount * 70));
	}
}
