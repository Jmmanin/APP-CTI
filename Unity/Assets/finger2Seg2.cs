using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class finger2Seg2 : MonoBehaviour {
	public finger2Seg3 seg;

	// Use this for initialization
	void Start () {
		seg = FindObjectOfType(typeof(finger2Seg3)) as finger2Seg3;
	}
	
	// Update is called once per frame
	void Update () {
		
	}

	public void Bend (float first, float amount, Vector3 palm) {
		float second = amount * 100;
		transform.rotation = Quaternion.Euler(new Vector3(0, 0, palm.x + 90 + first + second));
		seg.Bend (first + second, amount, palm);
	}
}
