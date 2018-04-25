using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class finger3 : MonoBehaviour {
	public finger3Seg1 seg;
	public palm palm;
	private float bend;

	// Use this for initialization
	void Start () {
		seg = FindObjectOfType(typeof(finger3Seg1)) as finger3Seg1;
		palm = FindObjectOfType(typeof(palm)) as palm;
	}

	// Update is called once per frame
	void Update () {
		seg.Bend (bend, palm.transform.rotation.eulerAngles);
	}

	public void BendFinger (float amount) {
		bend = amount;
	}
}
